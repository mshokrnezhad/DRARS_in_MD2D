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

int main(int argc, char* argv[])
{
	int cm_n = atoi(argv[1]);
	int cm_noise = atoi(argv[2]);
	int cm_max_power = atoi(argv[3]);
	const int n = cm_n + 1;
	double noise = cm_noise; //0.0000000001;
	double max_power = cm_max_power;

	int temp_next_node[n][n];
	int next_node[n];
	double tp[n];
	double rp[n];
	int nn_flag[n];
	double sir[n][n];
	double I[n];
	double temp_min;
	int child_node;
	int parent_node;

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

	int start_s = clock();

	for (int i = 0; i < n; i++)
	{
		temp_next_node[0][i] = 0;
		nn_flag[i] = 0;
	}

	for (int r = 0; r < n-1; r++)
	{
		for (int i = 1; i < n; i++)
		{
			tp[i] = max_power;
			rp[i] = max_power*h(x[i], x[temp_next_node[r][i]], y[i], y[temp_next_node[r][i]]);
		}

		sir[r][0] = 0;
		for (int i = 1; i < n; i++)
		{
			if (nn_flag[i] == 0)
			{

				I[i] = 0;
				for (int j = 1; j < n; j++)
					if (j != i)
						I[i] = I[i] + rp[j];
				I[i] = I[i] + noise;
				sir[r][i] = rp[i] / I[i];
			}
			else
				sir[r][i] = 0;
		}

		temp_min = 0;
		for (int tr = 0; tr <= r; tr++)
			for (int ti = 0; ti < n; ti++)
				if (nn_flag[ti] == 0 && sir[tr][ti] > temp_min)
				{
					temp_min = sir[tr][ti];
					child_node = ti;
					parent_node = temp_next_node[tr][ti];
				}

		next_node[child_node] = parent_node;
		nn_flag[child_node] = 1;

		for (int i = 1; i < n; i++)
		{
			if (nn_flag[i] == 0)
				temp_next_node[r + 1][i] = child_node;
			else
				temp_next_node[r + 1][i] = temp_next_node[r][i];
		}

	}

	// writing next nodes to file
	ofstream file_NN;
	file_NN.open("R01_TNN.txt", std::ios::app);
	for (int i = 1; i < n; i++)
	{
		file_NN << i << " " << next_node[i] << "\n";
	}
	file_NN << n << " 0";
	file_NN.close();

	return 0;
}
