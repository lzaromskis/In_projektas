/*
* Lygiagretaus programavimo
* inžinerinis projektas
* Lukas Žaromskis IFF-7/5
*
* graph.cpp
*/

#include "graph.h"
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <omp.h>
#include <math.h>
#include <iostream>
#include "gnuplot_i.hpp"

#include <iostream>

namespace lukzar
{
	Graph::Graph()
	{
		_points = std::vector<Vector2D>();
		_newPoints = std::vector<Vector2D>();

		_canGivePoint = false;
		_optimizationComplete = false;
		_pointsInserted = 0;

		_pointToGive = -1;

		_lockGet = new omp_lock_t();
		_lockPut = new omp_lock_t();
		omp_init_lock(_lockGet);
		omp_init_lock(_lockPut);
	}

	Graph::Graph(std::vector<Vector2D> points)
	{
		_points = std::vector<Vector2D>(points);
		_newPoints = std::vector<Vector2D>();

		_canGivePoint = false;
		_optimizationComplete = false;
		_pointsInserted = 0;

		_pointToGive = -1;

		_lockGet = new omp_lock_t();
		_lockPut = new omp_lock_t();
		omp_init_lock(_lockGet);
		omp_init_lock(_lockPut);
	}

	Graph::~Graph()
	{
		omp_destroy_lock(_lockGet);
		omp_destroy_lock(_lockPut);
		delete _lockGet;
		delete _lockPut;
	}

	void Graph::optimizeThread()
	{
		// Tęsti darbą, kol bus pilnai
		// optimizuotas grafikas
		while (!_optimizationComplete)
		{
			optimizeWork();
		}
	}

	void Graph::optimizeMain()
	{
		// Apskaičiuojama tikslo funkcijos reikšmė
		double targetOld = target(_points);
		// Vykdyti tol, kol grafikas bus pilnai optimizuotas
		while (!_optimizationComplete)
		{
			// Iteracijos pradžioje nustatomi tam tikri kintamieji
			// Pirmasis taškas turėtų būti (0; 0), todėl yra
			// nustatoma, kad vienas taškas į _newPoints yra įterptas
			// Taškai dalinti taip pat yra pradedami nuo indekso 1
			_pointsInserted = 1;
			_newPoints = std::vector<Vector2D>(_points.size());
			_pointToGive = 1;
			_canGivePoint = true;
			// Vykdo optimizacija per visus grafiko taškus
			while (_pointsInserted != _points.size())
			{
				optimizeWork();
			}
			// Apskaičiuojama nauja tikslo funkcijos reikšmė
			double targetNew = target(_newPoints);
			// Jeigu nauja tikslo funkcijos reikšmė yra mažesnė
			// už senąją, tai pakeisti senus taškus naujais ir
			// atnaujinti tikslo funkcijos reikšmę
			// Kitu atveju paskelbti, kad optimizavimas yra baigtas
			if (targetOld > targetNew)
			{
				_points = std::vector<Vector2D>(_newPoints);
				targetOld = targetNew;
			}
			else
			{
				_optimizationComplete = true;
			}
		}
	}

	void Graph::optimizeWork()
	{
		// Pabandyti pasiimti tašką. Jei gautasis indeksas
		// lygus -1, baigti.
		int index;
		Vector2D* v = getPoint(&index);
		if (index == -1)
			return;
		// Surasti gradientą ir nustatyti naują tašką 
		// priešinga gradientui kryptimi
		Vector2D grad = gradient(*v);
		Vector2D delta = grad.Multiply(1.0 / grad.Length()).Multiply(C_STEP);
		setNewPoint(Vector2D(v->x() - delta.x(), v->y() - delta.y()), index);
	}

	Vector2D* Graph::getPoint(int *index)
	{
		// Įėjimas į kritinę sekciją
		omp_set_lock(_lockGet);
			if (_canGivePoint)
			{	
				// Galima duoti tašką, tai jį duoti ir patikrinti ar
				// bus galima duoti kitą tašką
				*index = _pointToGive++;
				_canGivePoint = !(_pointToGive == _points.size());
				// Išėjimas iš kritinės sekcijos
				omp_unset_lock(_lockGet);
				return &_points[*index];
			}
			else
			{
				// Taško duoti negalima, tai reikia indeksą
				// nustatyti į -1
				// Išėjimas iš kritinės sekcijos
				omp_unset_lock(_lockGet);
				*index = -1;
				return nullptr;
			}
	}

	void Graph::setNewPoint(const Vector2D& point, const int &index)
	{
		// Nustatyti naują tašką
		_newPoints[index] = Vector2D(point);
		// Įėjimas į kritinę sekciją
		omp_set_lock(_lockPut);
		_pointsInserted++;
		// Išėjimas iš kritinės sekcijos
		omp_unset_lock(_lockPut);
	}

	Vector2D Graph::gradient(const Vector2D& point)
	{
		// Apskaičiuojamas gradiento vektorius
		Vector2D grad = Vector2D(0.0, 0.0);
		for (auto p : _points)
		{
			Vector2D v = Vector2D(p.x() - point.x(), p.y() - point.y());
			double length = v.Length();
			double sub = length - C_ALPHA;
			double f = pow(sub, 2.0);
			v = v.Normalize();
			double sign = signbit(sub) ? 1.0 : -1.0;
			grad = grad.Add(v.Multiply(f).Multiply(sign));
		}
		return grad.Normalize();
	}

	double Graph::target(const std::vector<Vector2D>& points)
	{
		// Apskaičiuojama tikslo funkcija
		double val = 0.0;
		for (int i = 0; i < points.size(); i++)
			for (int j = i + 1; j < _points.size(); j++)
				val += pow(Vector2D(points[i].x() - points[j].x(), points[i].y() - points[j].y()).Length() - C_ALPHA, 2.0);
		return val;
	}

	void Graph::SetPoints(std::vector<Vector2D> points)
	{
		_points = std::vector<Vector2D>(points);
		_newPoints = std::vector<Vector2D>();

		_canGivePoint = false;
		_optimizationComplete = false;
		_pointsInserted = 0;

		_pointToGive = -1;
	}

	void Graph::Optimize(int threads)
	{
		// Nustatomas gijų kiekis
		omp_set_num_threads(threads);
#pragma omp parallel
		{
			// Pagrindinė gija keliauja į savo funkcija
			// Visos kitos gijos yra darbininkės
			if (omp_get_thread_num() == 0)
				optimizeMain();
			else
				optimizeThread();
		}
	}

	double Graph::Target()
	{
		return target(_points);
	}

	void Graph::DrawGraph(std::string filename, int resolution)
	{
		std::ofstream filePoints("p.dat");
		std::ofstream fileLines("l.dat");
		for (int i = 0; i < _points.size(); i++)
		{
			std::string point1 = std::to_string(_points[i].x()) + " " + std::to_string(_points[i].y()) + "\n";
			for (int j = i + 1; j < _points.size(); j++)
			{
				std::string point2 = std::to_string(_points[j].x()) + " " + std::to_string(_points[j].y()) + "\n";

				fileLines.write(point1.c_str(), point1.size());
				fileLines.write(point2.c_str(), point2.size());
				fileLines.write("\n", 1);
			}
			filePoints.write(point1.c_str(), point1.size());
		}
		filePoints.close();
		fileLines.close();

		Gnuplot gp;
		gp.set_GNUPlotPath("./");
		gp << "set terminal png size " + std::to_string(resolution) + "," + std::to_string(resolution);
		gp << "set output '" + filename + "'";
		gp << "set xrange[-10:10]";
		gp << "set yrange[-10:10]";
		gp << "set grid";
		gp << "plot'l.dat' with lines lc rgb '#FF404040' notitle, \\";
		gp << "'p.dat' pt 7 lc rgb '#FFFF2020' notitle";


	}

	void Graph::PrintPoints()
	{
		for (auto p : _points)
		{
			std::cout << p.x() << "  " << p.y() << std::endl;
		}
	}
}