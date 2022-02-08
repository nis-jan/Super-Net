#include "id_list.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

id_list::id_list(int size) {
	internal_list = (int*)malloc(sizeof(int) * size);
	_size = size;
	_index = 1;
}

id_list::~id_list()
{
	free(internal_list);
}

void id_list::push(int obj) {
	move();
	internal_list[0] = obj;
	if(_index < _size)
		_index++;
}


void id_list::move() {
	for (int i = _index - 1; i > 0; i--) {
		internal_list[i] = internal_list[i - 1];
	}
}

bool id_list::contains(int obj) {
	for (int i = 0; i < _index; i++) {
		if (internal_list[i] == obj) return true;
	}

	return false;
}

int id_list::count() {
	return _index;
}

int id_list::size() {
	return _size;
}

void id_list::print() {
	cout << "|";
	for (int i = 0; i < _index; i++) {
		cout << internal_list[i] << "|";
	}
	cout << endl;
}