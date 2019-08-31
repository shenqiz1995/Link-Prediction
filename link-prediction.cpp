// link-prediction.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"
#include "math.h"
#include "time.h"
#include "iostream"
#include "queue"
#include "list"
#include "stack"
using namespace std;
#define MAXN1 6000
#define MAXN2 20000


FILE *fp;

int N;      //����Ľ����
int node_begin;      //��С�Ľ����
int M;      //�����������
int M_train;      //ѵ������������
int M_test;      //���Լ���������

typedef struct linked_list
{
	int node_num;      //�����     
	int edge_num;      //���߱��
	struct linked_list *next;
} Linked_List;
Linked_List *adjacency_list[MAXN1];

typedef struct couple
{
	bool connected;
	double self_information;      //����Ϣ��
	double conditional_information_O;      //����������Ϣ��
	double conditional_information_P;      //����������Ϣ��
	double NSI;      //NSIֵ
} Couple;
Couple couple[MAXN1][MAXN1];

typedef struct node
{
	int degree;      //��
	double cluster;      //����ϵ��
	double conditional_information;      //������Ϣ��
} Node;
Node node[MAXN1];

typedef struct edge
{
	int node1_num;
	int node2_num;
	bool valid;      //��Ϊ1���������������ѵ��������Ϊ0��������������ڲ��Լ�
	double cluster;      //����ϵ��
	double conditional_information;      //������Ϣ��
										 //	double mutual_information;      //����Ϣ��
} Edge;
Edge edge[MAXN2];

struct
{
	int node1_num;
	int node2_num;
	double NSI;
} edge_predicted[MAXN2 / 10 + 1];

double precision;


//�������������������
void network_connect(int node1, int node2)
{
	Linked_List *p;
	p = (Linked_List*)malloc(sizeof(Linked_List));
	p->node_num = node2;
	p->edge_num = M;
	p->next = adjacency_list[node1];
	adjacency_list[node1] = p;

	p = (Linked_List*)malloc(sizeof(Linked_List));
	p->node_num = node1;
	p->edge_num = M;
	p->next = adjacency_list[node2];
	adjacency_list[node2] = p;

	couple[node1][node2].connected = 1;
	couple[node2][node1].connected = 1;

	node[node1].degree++;
	node[node2].degree++;

	if (node1 < node2)
	{
		edge[M].node1_num = node1;
		edge[M].node2_num = node2;
		edge[M].valid = 1;
	}
	else
	{
		edge[M].node1_num = node2;
		edge[M].node2_num = node1;
		edge[M].valid = 1;
	}
}


//���ļ��ж�ȡ����
void fscan_network()
{
	int i;
	Linked_List *p;
	int node1, node2;
	double weight;
	bool flag;
	FILE *fp;
	fp = fopen("network.txt", "r");
	N = 0;
	M = 0;
	node_begin = 1;

	while (1)
	{
		fscanf(fp, "%d%d%lf", &node1, &node2, &weight);

		//�ж��Ƿ�Ϊ�յ�
		if (node1 == MAXN2&&node2 == MAXN2) break;

		//��ֹ����
		if (couple[node1][node2].connected == 1)
		{
			printf("�ظ����� ");
			continue;
		}

		//��ֹ�Ի�
		if (node1 == node2)
		{
			printf("�Ի� ");
			continue;
		}

		//�������ݽṹ
		network_connect(node1, node2);

		//��������,����������ʼ���
		if (N < node1 + 1) N = node1 + 1;
		if (N < node2 + 1) N = node2 + 1;
		M++;
		if (node1 == 0 || node2 == 0) node_begin = 0;
	}
	fclose(fp);
	printf("�ɹ���ȡ����\n");
}


//�ж������Ƿ�ȫ��ͨ
bool fully_connected(int source)
{
	Linked_List *p;
	bool visited[MAXN1] = { 0 };
	queue<int> Q;
	int node_num;
	int node_count = 0;
	visited[source] = 1;
	Q.push(source);
	while (!Q.empty())
	{
		node_num = Q.front();
		p = adjacency_list[node_num];
		Q.pop();
		node_count++;
		while (p != NULL)
		{
			if (edge[p->edge_num].valid && !visited[p->node_num])
			{
				Q.push(p->node_num);
				visited[p->node_num] = 1;
			}
			p = p->next;
		}
	}
	if (node_count < N - node_begin) return 0;
	else return 1;
}


//�����绮��Ϊѵ�����Ͳ��Լ�
void divide_network(double train_proportion)
{
	int i;
	int test_num;
	int test_count = 0;
	int edge_num;
	Linked_List *p;
	M_test = (int)((1 - train_proportion)*M + 0.5);
	M_train = M - M_test;
	while (test_count < M_test)
	{
		edge_num = rand() % M;
		if (edge[edge_num].valid)
		{
			edge[edge_num].valid = 0;
			if (fully_connected(edge[edge_num].node1_num))
			{
				test_count++;
				node[edge[edge_num].node1_num].degree--;
				node[edge[edge_num].node2_num].degree--;
				couple[edge[edge_num].node1_num][edge[edge_num].node2_num].connected = 0;
				couple[edge[edge_num].node2_num][edge[edge_num].node1_num].connected = 0;
			}
			else
			{
				//				printf("����ͨ ");
				edge[edge_num].valid = 1;
			}
		}
	}
	printf("�ɹ�����ѵ��������Լ�\n");
}


//��һ�����ߵ��������ҵ����ߵ����
int find_edge_num(int node1, int node2)
{
	int i;
	int edge_num = M;      //���û���ҵ�������ΪM
	if (node1 > node2)
	{
		int node3;
		node3 = node1;
		node1 = node2;
		node2 = node3;
	}
	for (i = 0; i < M; i++)
	{
		if (edge[i].node1_num == node1&&edge[i].node2_num == node2)
		{
			edge_num = i;
			break;
		}
	}
	return edge_num;
}


//����precision
void calculate_precision(double lambda)
{
	int i, j, k;
	precision = 0;
	for (i = 0; i < M_test; i++)
	{
		double NSI_max = -MAXN1*1.0;
		int node1, node2;
		for (j = node_begin; j < N; j++)
		{
			for (k = j + 1; k < N; k++)
			{
				if (couple[j][k].connected == 0)
				{
					if (couple[j][k].NSI > NSI_max)
					{
						node1 = j;
						node2 = k;
						NSI_max = couple[j][k].NSI;
					}
				}
			}
		}
		edge_predicted[i].node1_num = node1;
		edge_predicted[i].node2_num = node2;
		edge_predicted[i].NSI = NSI_max;
		couple[node1][node2].NSI = -MAXN1*1.0;
		if (find_edge_num(node1, node2) < M)
		{
			if (edge[find_edge_num(node1, node2)].valid == 0)
				precision += 1.0;
		}
	}
	precision /= M_test;
	printf("%lf(%lf)\n", precision, lambda);
	fprintf(fp, "%lf\n", precision);
}


//����NSI
void calculate_NSI()
{
	int i, j, k;
	Linked_List *p, *q;
	double lambda;

	//����I(Lxy|z)
	for (i = node_begin; i < N; i++)
	{
		if (node[i].degree > 1)
		{
			int Nc = 0;
			p = adjacency_list[i];
			while (p != NULL)
			{
				if (edge[p->edge_num].valid == 1)
				{
					q = adjacency_list[i];
					while (q != NULL)
					{
						if (edge[q->edge_num].valid == 1)
						{
							if (couple[p->node_num][q->node_num].connected == 1)
								Nc++;
						}
						q = q->next;
					}
				}
				p = p->next;
			}
			node[i].cluster = Nc*1.0 / node[i].degree / (node[i].degree - 1);
			if (node[i].cluster > 0) node[i].conditional_information = -log2(node[i].cluster);
		}
	}

	//����I(Lxy|lst)
	for (i = 0; i < M; i++)
	{
		if (edge[i].valid == 1)
		{
			double Nc = 0;
			double Nnc = 0;
			p = adjacency_list[edge[i].node1_num];
			while (p != NULL)
			{
				if (edge[p->edge_num].valid == 1 && p->node_num != edge[i].node2_num)
				{
					q = adjacency_list[edge[i].node2_num];
					while (q != NULL)
					{
						if (edge[q->edge_num].valid == 1 && q->node_num != edge[i].node1_num)
						{
							if (couple[p->node_num][q->node_num].connected == 1)
							{
								if (couple[p->node_num][edge[i].node2_num].connected == 1 && couple[q->node_num][edge[i].node1_num].connected == 1)
									Nc += 0.5;
								else
									Nc += 1.0;
							}
							else
							{
								if (p->node_num != q->node_num)
								{
									if (couple[p->node_num][edge[i].node2_num].connected == 1 && couple[q->node_num][edge[i].node1_num].connected == 1)
										Nnc += 0.5;
									else
										Nnc += 1.0;
								}
							}
						}
						q = q->next;
					}
				}
				p = p->next;
			}
			edge[i].cluster = Nc / (Nc + Nnc);
			if (edge[i].cluster > 0) edge[i].conditional_information = -log2(edge[i].cluster);
		}
	}

	//����I(Lxy),I(Lxy|Oxy)��I(Lxy|Pxy)
	for (i = node_begin; i < N; i++)
	{
		//		printf("%d\n", i);
		for (j = i + 1; j < N; j++)
		{
			if (couple[i][j].connected == 0)
			{
				//����I(Lxy)
				double P_disconnected = 1;
				for (k = 0; k < node[i].degree; k++)
				{
					P_disconnected *= (M_train - node[i].degree - node[j].degree + 1 + k)*1.0;
					P_disconnected /= (M_train - node[i].degree + 1 + k)*1.0;
				}
				couple[i][j].self_information = -log2(1 - P_disconnected);
				couple[j][i].self_information = -log2(1 - P_disconnected);

				//����I(Lxy|Oxy)��I(Lxy|Pxy)
				couple[i][j].conditional_information_O = couple[i][j].self_information;
				couple[j][i].conditional_information_O = couple[j][i].self_information;
				couple[i][j].conditional_information_P = couple[i][j].self_information;
				couple[j][i].conditional_information_P = couple[j][i].self_information;
				p = adjacency_list[i];
				while (p != NULL)
				{
					if (edge[p->edge_num].valid == 1)
					{
						q = adjacency_list[j];
						while (q != NULL)
						{
							if (edge[q->edge_num].valid == 1)
							{
								//�ҵ�z
								if (p->node_num == q->node_num)
								{
									if (node[p->node_num].cluster > 0)
									{
										couple[i][j].conditional_information_O -= (couple[i][j].self_information - node[p->node_num].conditional_information);
										couple[j][i].conditional_information_O -= (couple[j][i].self_information - node[p->node_num].conditional_information);
									}
								}

								//�ҵ�lst
								if (couple[p->node_num][q->node_num].connected == 1)
								{
									if (edge[find_edge_num(p->node_num, q->node_num)].cluster > 0)
									{
										if (couple[p->node_num][j].connected == 1 && couple[q->node_num][i].connected == 1)
										{
											couple[i][j].conditional_information_P -= 0.5*(couple[i][j].self_information - edge[find_edge_num(p->node_num, q->node_num)].conditional_information);
											couple[j][i].conditional_information_P -= 0.5*(couple[j][i].self_information - edge[find_edge_num(p->node_num, q->node_num)].conditional_information);
										}
										else
										{
											couple[i][j].conditional_information_P -= (couple[i][j].self_information - edge[find_edge_num(p->node_num, q->node_num)].conditional_information);
											couple[j][i].conditional_information_P -= (couple[j][i].self_information - edge[find_edge_num(p->node_num, q->node_num)].conditional_information);
										}
									}
								}
							}
							q = q->next;
						}
					}
					p = p->next;
				}
			}
		}
	}

	//����NSI
	lambda = 0.001;
	for (i = 0; i < 7; i++)
	{
		for (j = node_begin; j < N; j++)
		{
			for (k = j + 1; k < N; k++)
			{
				if (couple[j][k].connected == 0)
				{
					couple[j][k].NSI = -couple[j][k].conditional_information_O - lambda*couple[j][k].conditional_information_P;
					couple[k][j].NSI = -couple[k][j].conditional_information_O - lambda*couple[k][j].conditional_information_P;
				}
			}
		}
		calculate_precision(lambda);
		lambda *= 10;
	}
}


//���²���
void refresh()
{
	int i, j;
	for (i = 0; i < M; i++)
	{
		edge[i].cluster = 0;
		if (edge[i].valid == 0)
		{
			node[edge[i].node1_num].degree++;
			node[edge[i].node2_num].degree++;
			couple[edge[i].node1_num][edge[i].node2_num].connected = 1;
			couple[edge[i].node2_num][edge[i].node1_num].connected = 1;
			edge[i].valid = 1;
		}
	}

	for (i = node_begin; i < N; i++)
	{
		node[i].cluster = 0;
		node[i].conditional_information = 0;
	}

	for (i = node_begin; i < N; i++)
	{
		for (j = i + 1; j < N; j++)
		{
			couple[i][j].self_information = 0;
			couple[j][i].self_information = 0;
			couple[i][j].conditional_information_O = 0;
			couple[j][i].conditional_information_O = 0;
			couple[i][j].conditional_information_P = 0;
			couple[j][i].conditional_information_P = 0;
			couple[i][j].NSI = 0;
			couple[j][i].NSI = 0;
		}
	}
	printf("�ɹ����²���\n");
}


//���ж��ʵ��
void test()
{
	int i, j;
	int test_time = 10;
	double precision_random;
	fscan_network();
	srand((unsigned)time(NULL));
	for (i = 0; i < test_time; i++)
	{
		printf("��%d�β��ԣ�\n", i + 1);
		fprintf(fp, "��%d�β��ԣ�\n", i + 1);
		divide_network(0.9);
		calculate_NSI();
		refresh();
		printf("\n");
		fprintf(fp, "\n");
	}
	precision_random = M_test*1.0 / ((N - node_begin)*(N - node_begin - 1)*1.0 / 2 - M_train);
	fprintf(fp, "precision_random=%lf\n", precision_random);
}


int main()
{
	fp = fopen("precision.txt", "w");
	test();
	fclose(fp);
	return 0;
}



