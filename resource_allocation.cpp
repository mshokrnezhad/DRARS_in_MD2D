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

int main(int argc, char* argv[])
{
	int cm_n = atoi(argv[1]);
	int cm_no_specs = atoi(argv[2]);
	double cm_target_sinr = atof(argv[3]);
	double cm_noise = atof(argv[4]);
	double cm_max_power = atof(argv[5]);
	const int n = cm_n + 1;
	const int no_specs = cm_no_specs + 1;
	double target_sinr = cm_target_sinr; //0.05; //4;
	double noise = cm_noise; //0.0000000001;
	double max_power = cm_max_power;

	/*
	const int n = 10 + 1;
	const int no_specs = 5 + 1;
	double target_sinr = 0.5;
	double noise = 0.0000000001;
	double max_power = 2;
	*/

	const int round_bound = n; //minimun amount is 1 where we have just one round
	const int iteration_bound = 300;
	double rad = (1 / (2 * sqrt(0.1))) * cbrt(0.5 / 0.1);
	int center = 0;

	double sinr[round_bound][n][no_specs];
	double t_sinr_l[n][no_specs];
	double t_sinr_n[n][no_specs];
	double sum_sinr[round_bound];
	int sinr_received_counter = 0;
	double fixed_sinr[n];


	double p[round_bound][n][no_specs];
	double t_p_n[n][no_specs];
	double t_p_l[n][no_specs];
	double sum_power[round_bound];
	double fixed_power[n];


	double I[round_bound][n][no_specs];
	double t_I_n[n][no_specs];
	double t_I_l[n][no_specs];

	double outage_ratio[round_bound];

	int max_powered_node;
	int min_interfered_channel;

	int round_loop_index_counter = 0;
	int round_loop_flag = 0;

	int next_node[n];
	int min_distance;
	int parent_node;
	int child_node;
	int chflag[n];
	int round;
	int channel[round_bound][n];
	int step_channel[n][n];
	double temp[n + no_specs];

	int iteration_loop_index_counter = 0;
	int r_counter = 0;
	int iteration_loop_flag = 0;
	int iteration;

	double x[n];
	double y[n];

	double noh[n];

	int pgsan[n];
	int step[n];
	int s = 1;

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
	ifstream nnfile;
	nnfile.open("R01_NN.txt");
	while (!nnfile.eof())
	{
		nnfile >> temp_a >> temp_b;
		if (temp_a != n)
			next_node[temp_a] = temp_b;
		else
			next_node[0] = temp_b;
	}
	nnfile.close();
	next_node[0] = 0;

	temp_a = 0;
	temp_b = 0;
	ifstream nohfile;
	nohfile.open("R01_NoH.txt");
	while (!nohfile.eof())
	{
		nohfile >> temp_a >> temp_b;
		if (temp_a != n)
			noh[temp_a] = temp_b;
		else
			noh[0] = temp_b;
	}
	nohfile.close();
	noh[0] = n;

	int start_s = clock();

	for (int i = 1; i < n; i++)
	{
		pgsan[i] = func_get_min_index(noh, n);
		noh[pgsan[i]] = n;
	}
	pgsan[0] = 0;

	step[0] = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 1; j < n; j++)
			if (next_node[j] == pgsan[i])
				step[j] = s;
		s++;
	}

	for (int i = 1; i < n; i++)
	{
		fixed_power[i] = 0;
		fixed_sinr[i] = 0;
		for (int r = 0; r < round_bound; r++)
		{
			channel[r][i] = 0;// temp_nu%no_specs;
			chflag[i] = 0;
		}
	}

	for (int s = 1; s < n; s++)
	{
		round = 0;
		round_loop_flag = 0;
		//while (sinr_received_counter != no_trans && round<round_bound && round_loop_flag != 1)
		while (round < round_bound && round_loop_flag != 1)
		{
			// first assignment of channel and power to all nodes
			if (round == 0)
				for (int i = 1; i < n; i++)
				{
					if (chflag[i] == 1)
						channel[round][i] = step_channel[s - 1][i];// temp_nu%no_specs;
					if (step[i] == s)
					{
						channel[round][i] = 1;// temp_nu%no_specs;
						center = next_node[i];
					}
				}
			else
			{
				// finding max_interfered_node

				for (int i = 0; i < n + no_specs; i++)
					temp[i] = 0;

				for (int i = 1; i < n; i++)
					if (chflag[i] == 0 && distance(x[center], x[i], y[center], y[i]) <= rad)
						temp[i] = p[round - 1][i][channel[round - 1][i]];
					else
						temp[i] = 0;
				max_powered_node = func_get_max_index(temp, n);

				// finding min_interfered_channel
				for (int k = 1; k < no_specs; k++)
					temp[k - 1] = I[round - 1][max_powered_node][k];
				min_interfered_channel = func_get_min_index(temp, no_specs - 1) + 1;

				for (int i = 1; i < n; i++)
				{
					// assigning channel and power to max_interfered_node
					if (i == max_powered_node)
						channel[round][i] = min_interfered_channel;

					// assigning channel and power to other nodes
					else
						channel[round][i] = channel[round - 1][i];
				}
			}

			// power control procedure
			iteration = 0;
			for (int i = 1; i < n; i++)
			{
				if (distance(x[center], x[i], y[center], y[i]) <= rad)
				{
					for (int k = 0; k < no_specs; k++)
						t_p_n[i][k] = 0;
					if (channel[round][i] == 0)
						t_p_n[i][channel[round][i]] = 0;
					else
						t_p_n[i][channel[round][i]] = max_power;
				}
			}

			iteration_loop_flag = 0;

			// power control itterations
			while (iteration < iteration_bound)// && iteration_loop_flag != 1)
			{
				if (iteration != 0)
					for (int i = 1; i < n; i++)
					{
						if (distance(x[center], x[i], y[center], y[i]) <= rad)
						{
							for (int k = 1; k < no_specs; k++)
								t_p_n[i][k] = 0;

							if (channel[round][i] == 0)
								t_p_n[i][channel[round][i]] = 0;

							else
								t_p_n[i][channel[round][i]] = min(max_power, target_sinr*(t_p_l[i][channel[round][i]] / t_sinr_l[i][channel[round][i]]));
						}
					}

				for (int i = 1; i < n; i++)
					if (distance(x[center], x[i], y[center], y[i]) <= rad)
						for (int k = 0; k < no_specs; k++)
						{
							t_I_n[i][k] = 0;
							for (int j = 1; j < n; j++)
								if (j != i && distance(x[center], x[j], y[center], y[j]) <= rad)
									t_I_n[i][k] = t_I_n[i][k] + t_p_n[j][k] * h(x[j], x[next_node[i]], y[j], y[next_node[i]]);
							t_I_n[i][k] = t_I_n[i][k] + noise;
							t_sinr_n[i][k] = (t_p_n[i][k] * h(x[i], x[next_node[i]], y[i], y[next_node[i]])) / t_I_n[i][k];
						}

				iteration_loop_index_counter = 0;

				/*for (int i = 1; i < n; i++)
				if (diff(t_sinr_l[i][channel[round][i]], t_sinr_n[i][channel[round][i]]) == 0)
				iteration_loop_index_counter++;
				if (iteration_loop_index_counter == (n - 1))
				iteration_loop_flag = 1;*/

				for (int i = 1; i < n; i++)
					if (distance(x[center], x[i], y[center], y[i]) <= rad)
						for (int k = 0; k < no_specs; k++)
						{
							t_p_l[i][k] = t_p_n[i][k];
							t_I_l[i][k] = t_I_n[i][k];
							t_sinr_l[i][k] = t_sinr_n[i][k];
						}

				iteration++;
			}

			for (int i = 1; i < n; i++)
			{
				if (distance(x[center], x[i], y[center], y[i]) <= rad)
				{
					for (int k = 0; k < no_specs; k++)
					{
						p[round][i][k] = t_p_l[i][k];
						I[round][i][k] = t_I_l[i][k];
						sinr[round][i][k] = t_sinr_l[i][k];
						fixed_power[i] = t_p_l[i][channel[round][i]];
						fixed_sinr[i] = t_sinr_l[i][channel[round][i]];
					}
				}
				/*else
				for (int k = 0; k < no_specs; k++)
				{
				p[round][i][k] = p[round-1][i][k];
				I[round][i][k] = I[round-1][i][k];
				sinr[round][i][k] = sinr[round-1][i][k];
				}*/
			}

			//**********************************************************************************************************************************
			//Without this line, the program was executed in MS. VS	perfectly but in Linux this part should be added to produce flawless results		
			for (int i = 1; i < n; i++)
				p[round][i][0] = 0;
			//**********************************************************************************************************************************

			// final values of each power control procedure
			sinr_received_counter = 0;
			sum_sinr[round] = 0;
			sum_power[round] = 0;
			for (int i = 1; i < n; i++)
			{
				if (fixed_sinr[i] > target_sinr - 0.01)
					sinr_received_counter++;
				sum_sinr[round] = sum_sinr[round] + fixed_sinr[i];
				sum_power[round] = sum_power[round] + fixed_power[i];
			}
			outage_ratio[round] = 1 - (sinr_received_counter / (double)(n - 1));

			if (round > 0)
			{
				if (sum_power[round] > sum_power[round - 1])
					for (int i = 1; i < n; i++)
					{
						channel[round][i] = channel[round - 1][i];
						for (int k = 0; k < no_specs; k++)
						{
							I[round][i][k] = I[round - 1][i][k];
							p[round][i][k] = p[round - 1][i][k];
							sum_power[round] = sum_power[round - 1];
							sinr[round][i][k] = sinr[round - 1][i][k];
						}
					}
				chflag[max_powered_node] = 1;

			}

			round_loop_flag = 0;
			round_loop_index_counter = 0;
			for (int i = 1; i < n; i++)
				if (step[i] == s && chflag[i] == 1)
					round_loop_index_counter++;
			for (int i = 1; i < n; i++)
				if (step[i] == s)
					round_loop_index_counter--;
			if (round_loop_index_counter == 0)
				round_loop_flag = 1;

			round++;
		}

		for (int i = 1; i < n; i++)
			step_channel[s][i] = channel[round - 1][i];
	}

	int stop_s = clock();

	// writing Sum of Power to file
	ofstream file_SoP;
	file_SoP.open("R03_cpp_dec_SoP.txt", std::ios::app);
	file_SoP << sum_power[round - 1] << "\n";
	file_SoP.close();

	// writing Sum of SINR to file
	ofstream file_SoS;
	file_SoS.open("R03_cpp_dec_SoS.txt", std::ios::app);
	file_SoS << sum_sinr[round - 1] << "\n";
	file_SoS.close();

	//writingNumber of Satisfied Users to file
	ofstream file_et;
	file_et.open("R03_cpp_dec_OR.txt", std::ios::app);
	file_et << outage_ratio[round - 1] << endl;
	file_et.close();

	return 0;
}
