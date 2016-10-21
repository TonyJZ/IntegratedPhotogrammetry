#include "stdafx.h"
#include "GenPointIndex/Octreeindex.h"

#define PCL_NO_PRECOMPILE
#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/kdtree/impl/kdtree_flann.hpp>

#include <iostream>
#include <vector>
#include <ctime>


struct MyPointType
{
	double x, y, z;                  // preferred way of adding a XYZ+padding
	uint32_t id;
	float padding;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
}EIGEN_ALIGN16; 


POINT_CLOUD_REGISTER_POINT_STRUCT (MyPointType,           // here we assume a XYZ + "test" (as fields)
	(double, x, x)
	(double, y, y)
	(double, z, z)
	(uint32_t, id, id)
	(float, padding, padding)
	)

COctreePtIndex::COctreePtIndex(orsIPlatform *platform)
{
	srand (time (NULL));

	pcl::PointCloud<MyPointType>::Ptr cloud (new pcl::PointCloud<MyPointType>);

	// Generate pointcloud data
	cloud->width = 1000;
	cloud->height = 1;
	cloud->points.resize (cloud->width * cloud->height);

	for (size_t i = 0; i < cloud->points.size (); ++i)
	{
		cloud->points[i].x = 1024.0f * rand () / (RAND_MAX + 1.0f);
		cloud->points[i].y = 1024.0f * rand () / (RAND_MAX + 1.0f);
		cloud->points[i].z = 1024.0f * rand () / (RAND_MAX + 1.0f);
	}

	pcl::KdTreeFLANN<MyPointType> kdtree;

	kdtree.setInputCloud (cloud);

	MyPointType searchPoint;

	searchPoint.x = 1024.0f * rand () / (RAND_MAX + 1.0f);
	searchPoint.y = 1024.0f * rand () / (RAND_MAX + 1.0f);
	searchPoint.z = 1024.0f * rand () / (RAND_MAX + 1.0f);

	// K nearest neighbor search

	int K = 10;

	std::vector<int> pointIdxNKNSearch(K);
	std::vector<float> pointNKNSquaredDistance(K);

	std::cout << "K nearest neighbor search at (" << searchPoint.x 
		<< " " << searchPoint.y 
		<< " " << searchPoint.z
		<< ") with K=" << K << std::endl;

	if ( kdtree.nearestKSearch (searchPoint, K, pointIdxNKNSearch, pointNKNSquaredDistance) > 0 )
	{
		for (size_t i = 0; i < pointIdxNKNSearch.size (); ++i)
			std::cout << "    "  <<   cloud->points[ pointIdxNKNSearch[i] ].x 
			<< " " << cloud->points[ pointIdxNKNSearch[i] ].y 
			<< " " << cloud->points[ pointIdxNKNSearch[i] ].z 
			<< " (squared distance: " << pointNKNSquaredDistance[i] << ")" << std::endl;
	}

	// Neighbors within radius search

	std::vector<int> pointIdxRadiusSearch;
	std::vector<float> pointRadiusSquaredDistance;

	float radius = 256.0f * rand () / (RAND_MAX + 1.0f);

	std::cout << "Neighbors within radius search at (" << searchPoint.x 
		<< " " << searchPoint.y 
		<< " " << searchPoint.z
		<< ") with radius=" << radius << std::endl;


	if ( kdtree.radiusSearch (searchPoint, radius, pointIdxRadiusSearch, pointRadiusSquaredDistance) > 0 )
	{
		for (size_t i = 0; i < pointIdxRadiusSearch.size (); ++i)
			std::cout << "    "  <<   cloud->points[ pointIdxRadiusSearch[i] ].x 
			<< " " << cloud->points[ pointIdxRadiusSearch[i] ].y 
			<< " " << cloud->points[ pointIdxRadiusSearch[i] ].z 
			<< " (squared distance: " << pointRadiusSquaredDistance[i] << ")" << std::endl;
	}


}

COctreePtIndex::~COctreePtIndex()
{

}