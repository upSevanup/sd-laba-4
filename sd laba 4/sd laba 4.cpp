#include <iostream>
#include <clocale>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;

struct Node //структура для представления узлов дерева
{
	int key;
	unsigned char height;
	Node* left;
	Node* right;
	Node(int k) { key = k; left = right = 0; height = 1; }
};

unsigned char height(Node* p);
int bfactor(Node* p);
void fixheight(Node* p);
Node* rotateright(Node* p);
Node* rotateleft(Node* q);
Node* balance(Node* p);
Node* insert(Node* p, int k);
void print_tree(Node* node, ofstream& file);
Node* findmin(Node* p);
Node* removemin(Node* p);
Node* remove(Node* p, int k);
int get_disbalnace(Node* p, int* array, int n);
int delete_on_level(Node* head, Node* p, int lvl, int n);
bool in_list(int val, int* list, int n);
int delete_tree(Node* p);

int main() {
	setlocale(LC_ALL, "ru");
	srand(static_cast<unsigned int>(time(0)));
	ofstream inputfile("input.dat");
	ofstream outputfile("output.dat");

	//создание дерева (элементы дерева не повторяются)
	int n;
	cout << "Введите количество элементов дерева >> ";
	cin >> n;
	int* tree_elems = new int[n];
	int elem = (rand() % 100);
	Node* tree = new Node(elem);
	tree_elems[0] = elem;
	int i = 1;
	while (i < n) {
		elem = (rand() % 100);
		if (in_list(elem, tree_elems, n)) {
			insert(tree, elem);
			tree_elems[i] = elem;
			i++;
		}
	}

	cout << endl << "Исходное АВЛ-дерево: " << endl;
	inputfile << "количество элементов дерева: " << n << endl;
	inputfile << endl << "Исходное АВЛ-дерево: " << endl;
	print_tree(tree, inputfile);

	//находим среднее количество разбалансированных узлов на каждом уровне
	int h = height(tree);
	int* nodes = new int[h];
	for (int i = 0; i < h; i++) {
		nodes[i] = 0;
	}
	get_disbalnace(tree, nodes, 0);

	//находим среднее количество разбалансированных узлов 
	double mid = 0;
	for (int i = 0; i < h; i++) {
		mid += nodes[i];
	}
	mid /= h;

	cout << endl << "Среднее количество разбалансированных узлов: " << mid << endl;
	outputfile << "Среднее количество разбалансированных узлов: " << mid << endl;

	//находим уровень с максимальным отклонением
	int deviation = 0;
	int level = 0;
	for (int i = 0; i < h; i++) {
		if ((nodes[i] - mid) >= deviation) { 
			deviation = nodes[i] - mid;
			level = i;
		}
	}

	cout << endl << "уровень с наибольшим отклонением: " << level + 1 << endl;
	outputfile << endl << "уровень с наибольшим отклонением: " << level + 1 << endl;

	//удаляем разбалансированные элементы на уровне
	delete_on_level(tree, tree, level, 0);

	cout << endl << "АВЛ-дерево после удаления разбалансированных узлов уровня: " << endl;
	outputfile << endl << "АВЛ-дерево после удаления разбалансированных узлов уровня: " << endl;
	print_tree(tree, outputfile);

	//отчистка памяти
	delete_tree(tree);
}

bool in_list(int val, int* list, int n) {
	for (int i = 0; i < n; i++) {
		if (list[i] == val) {
			return false;
		}
	}
	return true;
}

unsigned char height(Node* p) {
	return p ? p->height : 0;
}

int bfactor(Node* p) {
	return height(p->right) - height(p->left);
}

void fixheight(Node* p) {
	unsigned char hl = height(p->left);
	unsigned char hr = height(p->right);
	p->height = (hl > hr ? hl : hr) + 1;
}

Node* rotateright(Node* p) {
	Node* q = p->left;
	p->left = q->right;
	q->right = p;
	fixheight(p);
	fixheight(q);
	return q;
}

Node* rotateleft(Node* q) {
	Node* p = q->right;
	q->right = p->left;
	p->left = q;
	fixheight(q);
	fixheight(p);
	return p;
}

Node* balance(Node* p) {
	fixheight(p);
	if (bfactor(p) == 2)
	{
		if (bfactor(p->right) < 0)
			p->right = rotateright(p->right);
		return rotateleft(p);
	}
	if (bfactor(p) == -2)
	{
		if (bfactor(p->left) > 0)
			p->left = rotateleft(p->left);
		return rotateright(p);
	}
	return p;
}

Node* insert(Node* p, int k) {
	if (!p) return new Node(k);
	if (k < p->key)
		p->left = insert(p->left, k);
	else
		p->right = insert(p->right, k);
	return balance(p);
}

void print_tree(Node* node, ofstream& file) {
	Node* ptr = node;
	int flag = 1, uv = -1;
	Node* st[100];

	while (flag == 1) {
		if (ptr == NULL) {
			if (uv == -1) {
				flag = 0;
			}
			else {
				ptr = st[uv--];
				ptr = ptr->right;
			}
		}
		else {
			printf("%4ld", ptr->key);
			file << ptr->key << "\t";

			if (ptr->left != NULL) {
				printf("%4ld", ptr->left->key);
				file << ptr->left->key << "\t";
			}
			else {
				printf("----");
				file << "----" << "\t";
			}

			if (ptr->right != NULL) {
				printf("%4ld", ptr->right->key);
				file << ptr->right->key << "\t";
			}
			else {
				printf("-----");
				file << "----" << "\t";
			}

			cout << endl;
			file << endl;
			st[++uv] = ptr;
			ptr = ptr->left;
		}
	}
}

Node* findmin(Node* p) {
	return p->left ? findmin(p->left) : p;
}

Node* removemin(Node* p) {
	if (p->left == 0)
		return p->right;
	p->left = removemin(p->left);
	return balance(p);
}

Node* remove(Node* p, int k) {
	if (!p) return 0;
	if (k < p->key)
		p->left = remove(p->left, k);
	else if (k > p->key)
		p->right = remove(p->right, k);
	else 
	{
		Node* q = p->left;
		Node* r = p->right;
		delete p;
		if (!r) return q;
		Node* min = findmin(r);
		min->right = removemin(r);
		min->left = q;
		return balance(min);
	}
	return balance(p);
}

int get_disbalnace(Node* p, int* array, int n) {
	if (!p) {
		return 0;
	}
	if (bfactor(p) == 1 or bfactor(p) == -1) {
		array[n] = array[n] + 1;
	}
	get_disbalnace(p->left, array, n + 1);
	get_disbalnace(p->right, array, n + 1);
}

int delete_on_level(Node* head, Node* p, int lvl, int n) {
	if (!p) {
		return 0;
	}
	if (n == lvl) {
		if (bfactor(p) == 1 or bfactor(p) == -1) {
			remove(head, p->key);
		}
		return 0;
	}
	delete_on_level(head, p->left, lvl, n + 1);
	delete_on_level(head, p->right, lvl, n + 1);
}

int delete_tree(Node* p) {
	if (!p) {
		return 0;
	}
	delete_tree(p->left);
	delete_tree(p->right);
	delete(p);
}