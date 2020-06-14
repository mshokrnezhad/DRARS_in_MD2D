// With noncooperative power update function
//
#include "iostream"
#include "fstream"
#include "iomanip"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "vector"
#include "ctime"

using namespace std;

double distance(double x_i, double x_j, double y_i, double y_j)
{
	return sqrt(pow((x_i - x_j), 2) + pow((y_i - y_j), 2));
}

double h(double x_i, double x_next_node_j, double y_i, double y_next_node_j)
{
	if (distance(x_i, x_next_node_j, y_i, y_next_node_j) == 0)
		return 1;
	else
		return 0.09*pow(distance(x_i, x_next_node_j, y_i, y_next_node_j), -3);
}

int random_generator(int min, int max)
{
	int random_number;
	//srand(time(NULL));
	random_number = rand() % (1000 - 0) + 0;
	for (int i = 0; i<max - min + 1; i++)
		if (random_number >= ((1000 * i) / (max - min + 1)) && random_number <= ((1000 * (i + 1)) / (max - min + 1)))
			return i + min;
}

void print_progress_bar(int percent)
{
	string bar;
	cout << "\r" << bar;
	cout << percent << "% " << std::flush;
}

double func_get_max_index(double arr[], int size)
{
	int MaxIndex;
	double temp_max = 0;
	for (int i = 0; i<size; i++)
		if (arr[i]>temp_max)
		{
			temp_max = arr[i];
			MaxIndex = i;
		}

	return MaxIndex;
}

double func_get_min_index(double arr[], int size)
{
	int MinIndex;
	double temp_min = 1000000000000;
	for (int i = 0; i<size; i++)
		if (arr[i]<temp_min)
		{
			temp_min = arr[i];
			MinIndex = i;
		}

	return MinIndex;
}

double func_get_max(double arr[], int size)
{
	int MaxIndex;
	double temp_max = 0;
	for (int i = 0; i<size; i++)
		if (arr[i]>temp_max)
		{
			temp_max = arr[i];
			MaxIndex = i;
		}

	return temp_max;
}

double func_get_min(double arr[], int size)
{
	int MinIndex;
	double temp_min = 1000000000000;
	for (int i = 0; i<size; i++)
		if (arr[i]<temp_min)
		{
			temp_min = arr[i];
			MinIndex = i;
		}

	return temp_min;
}

double diff(double a, double b)
{
	if (a >= b)
		return a - b;
	else
		return b - a;
}

int func_next_node(int i, int n, int r, double x[], double y[])
{
	int temp_distance = 1000000;
	int n_n = 0;
	if (i == 0)
		return n_n;
	else
	{
		for (int j = 0; j < i; j++)
			if (i != j)
				if (distance(x[i], x[j], y[i], y[j]) <= r)
					if (distance(x[j], x[0], y[j], y[0]) <= temp_distance)
					{
						temp_distance = distance(x[j], x[0], y[j], y[0]);
						n_n = j;
					}
		return n_n;
	}
}

int main(int argc, char* argv[])
{
	int cm_n = atoi(argv[1]);
	int cm_dr = atoi(argv[2]);
	int cm_cr = atoi(argv[3]);
	const int n = cm_n + 1;
	int dr = cm_dr; //delay requirment of nodes
	int cr = cm_cr; // node capacity

	int next_node[n];
	int rflag[n];
	int min_distance;
	int parent_node;
	int child_node;
	int nc[n]; // number of devices connected to relay i
	double noh[n];
	int noh_flag = 0;
	int hop_flag[n];
	int dr_flag[n];
	int cr_flag[n];

	int r_counter = 0;
	int tn_01 = 0; //temporary nodes
	int tn_02 = 0;
	int tn_03 = 0;

	double x[n];
	double y[n];

	int temp_a = 0;
	int temp_c = 0;
	double temp_b = 0;

	ifstream rxfile;
	rxfile.open("I01_x.txt");
	while (!rxfile.eof())
	{
		rxfile >> temp_a >> temp_b;
		if (temp_a != n)
			x[temp_a] = temp_b;
		else
			x[0] = temp_b;
	}
	rxfile.close();

	temp_a = 0;
	temp_b = 0;
	ifstream ryfile;
	ryfile.open("I01_y.txt");
	while (!ryfile.eof())
	{
		ryfile >> temp_a >> temp_b;
		if (temp_a != n)
			y[temp_a] = temp_b;
		else
			y[0] = temp_b;
	}
	ryfile.close();

	temp_a = 0;
	temp_b = 0;
	ifstream tnnfile;
	tnnfile.open("R01_TNN.txt");
	while (!tnnfile.eof())
	{
		tnnfile >> temp_a >> temp_b;
		if (temp_a != n)
			next_node[temp_a] = temp_b;
		else
			next_node[0] = temp_b;
	}
	tnnfile.close();

	int start_s = clock();

	//finding number of hopes
	noh_flag = 0;
	for (int i = 0; i < n; i++)
	{
		noh[i] = 0;
		hop_flag[i] = 0;
		dr_flag[i] = 0;
		cr_flag[i] = 0;
	}
	hop_flag[0] = 1;

	while (noh_flag != n - 1)
	{
		for (int j = 1; j < n; j++)
			if (hop_flag[j] != 1 && hop_flag[next_node[j]] == 1)
			{
				noh[j] = noh[next_node[j]] + 1;
				hop_flag[j] = 1;
				noh_flag++;
			}
	}

	//satisfing delay requirment
	for (int i = 1; i < n; i++)
	{
		dr_flag[i] = 0;
		while (dr_flag[i] == 0)
		{
			tn_01 = 0;
			tn_02 = 0;
			tn_03 = 0;

			if (noh[i]>dr)
			{
				min_distance = 100000;
				tn_01 = i;
				while (tn_01 != 0)
				{
					if (distance(x[tn_01], x[next_node[tn_01]], y[tn_01], y[next_node[tn_01]]) < min_distance && noh[tn_01] >= dr + 1)
					{
						min_distance = distance(x[tn_01], x[next_node[tn_01]], y[tn_01], y[next_node[tn_01]]);
						tn_02 = tn_01;
					}
					tn_01 = next_node[tn_01];
				}

				min_distance = 100000;
				for (int j = 0; j < n; j++)
					if (j != tn_02 && noh[j] < noh[tn_02] - 1)
						if (distance(x[tn_02], x[j], y[tn_02], y[j]) < min_distance)
						{
							min_distance = distance(x[tn_02], x[j], y[tn_02], y[j]);
							tn_03 = j;
						}
				next_node[tn_02] = tn_03;

				noh_flag = 0;
				for (int i = 0; i < n; i++)
				{
					noh[i] = 0;
					hop_flag[i] = 0;
				}
				hop_flag[0] = 1;

				while (noh_flag != n - 1)
				{
					for (int j = 1; j < n; j++)
						if (hop_flag[j] != 1 && hop_flag[next_node[j]] == 1)
						{
							noh[j] = noh[next_node[j]] + 1;
							hop_flag[j] = 1;
							noh_flag++;
						}
				}
			}
			if (noh[i] <= dr)
				dr_flag[i] = 1;
		}
	}

	//finding number of connected nodes to each relay
	for (int i = 0; i < n; i++)
		nc[i] = 0;

	for (int i = 0; i < n; i++)
	{
		for (int j = 1; j < n; j++)
			if (next_node[j] == i)
				nc[i]++;
	}
	nc[0] = 0;

	//satisfing capacity requirment
	for (int i = 1; i < n; i++)
	{
		cr_flag[i] = 0;
		while (cr_flag[i] == 0)
		{
			tn_01 = 0;
			tn_02 = 0;
			tn_03 = 0;

			if (nc[i]>cr)
			{
				min_distance = 100000;
				for (int j = 1; j < n; j++)
					if (next_node[j] == i && distance(x[i], x[j], y[i], y[j]) < min_distance)
					{
						min_distance = distance(x[i], x[j], y[i], y[j]);
						tn_01 = j;
					}

				min_distance = 100000;
				for (int j = 0; j < n; j++)
					if (j != tn_01 && noh[j] < noh[tn_01] && nc[j] < cr)
						if (distance(x[tn_01], x[j], y[tn_01], y[j]) < min_distance)
						{
							min_distance = distance(x[tn_02], x[j], y[tn_02], y[j]);
							tn_02 = j;
						}
				next_node[tn_01] = tn_02;

				noh_flag = 0;
				for (int i = 0; i < n; i++)
				{
					noh[i] = 0;
					hop_flag[i] = 0;
				}
				hop_flag[0] = 1;



				for (int i = 0; i < n; i++)
					nc[i] = 0;

				for (int i = 0; i < n; i++)
				{
					for (int j = 1; j < n; j++)
						if (next_node[j] == i)
							nc[i]++;
				}
				nc[0] = 0;



				while (noh_flag != n - 1)
				{
					for (int j = 1; j < n; j++)
						if (hop_flag[j] != 1 && hop_flag[next_node[j]] == 1)
						{
							noh[j] = noh[next_node[j]] + 1;
							hop_flag[j] = 1;
							noh_flag++;
						}
				}
			}
			
			if (nc[i] <= cr)
				cr_flag[i] = 1;
		}
	}

	int stop_s = clock();

	// writing next nodes to file
	ofstream file_NN;
	file_NN.open("R01_NN.txt", std::ios::app);
	for (int i = 1; i < n; i++)
	{
		file_NN << i << " " << next_node[i] << "\n";
	}
	file_NN << n << " 0";
	file_NN.close();

	// writing number of hops to file
	ofstream file_NoH;
	file_NoH.open("R01_NoH.txt", std::ios::app);
	for (int i = 1; i < n; i++)
	{
		file_NoH << i << " " << noh[i] << "\n";
	}
	file_NoH << n << " " << n;
	file_NoH.close();

	return 0;
}
