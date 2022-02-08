#pragma once
class id_list
{
public:
	id_list(int size);
	~id_list();
	void push(int obj);
	int size();
	int count();
	bool contains(int obj);
	void print();
private:
	int* internal_list;
	int _size;
	int _index;
	void move();
};

