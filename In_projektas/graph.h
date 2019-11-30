/*
* Lygiagretaus programavimo
* inžinerinis projektas
* Lukas Žaromskis IFF-7/5
*
* graph.h
*/
#pragma once

#include <vector>
#include <string>
#include <omp.h>
#include "vector2d.h"

namespace lukzar
{
	class Graph
	{
	private:
		std::vector<Vector2D> _points;
		std::vector<Vector2D> _newPoints;

		bool _canGivePoint;
		bool _optimizationComplete;
		int _pointsInserted;

		int _pointToGive;

		omp_lock_t* _lockGet;
		omp_lock_t* _lockPut;

		const double C_ALPHA = 3.0;
		const double C_STEP = 0.02;
		
		void optimizeThread();
		void optimizeMain();
		void optimizeWork();
		Vector2D* getPoint(int *index);
		void setNewPoint(const Vector2D &point, const int &index);
		Vector2D gradient(const Vector2D &point);
		double target(const std::vector<Vector2D> &points);

	public:
		Graph();
		Graph(std::vector<Vector2D> points);
		~Graph();
		
		void SetPoints(std::vector<Vector2D> points);
		void Optimize(int threads);
		double Target();
		void DrawGraph(std::string filename, int resolution);
		void PrintPoints();
	};


}