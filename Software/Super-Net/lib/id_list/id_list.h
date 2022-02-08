#ifndef IDLIST_H
#define IDLIST_H

/// <summary>
/// list of ints with fixed size, that automatically pops the element<br>
/// that is contained for the longes amount of time, when the list is full<br>
/// and a new element is supposed to be added <br>
/// is used to see which packets already have passed this node
/// </summary>
class id_list
{
public:
	/// <summary>
	/// constructor
	/// </summary>
	/// <param name="size">size of the id list</param>
	id_list(int size);
	
	/// <summary>
	/// destructor, deallocates memory, allocated by this list
	/// </summary>
	~id_list();

	/// <summary>
	/// pushes a new integer in the list<br> 
	/// and prevents overflowing
	/// </summary>
	/// <param name="obj">integer to be pushed into the list</param>
	void push(int obj);

	/// <summary>
	/// getter for the size of this list
	/// </summary>
	/// <returns>size of list</returns>
	int size();

	/// <summary>
	/// returns the actual amount of elements, that are pushed into this list
	/// </summary>
	/// <returns>actual amount of elements in the list</returns>
	int count();

	/// <summary>
	/// checks if a specified int is already in the list
	/// </summary>
	/// <param name="obj">int to be checked</param>
	/// <returns>true, if obj is already in the list, false if not</returns>
	bool contains(int obj);

	/// <summary>
	/// prints the contents of this list
	/// </summary>
	void print();
private:
	/// <summary>
	/// int array representing the list
	/// </summary>
	int* internal_list;
	/// <summary>
	/// size of the list
	/// </summary>
	int _size;
	/// <summary>
	/// actual amount of elements in this list
	/// </summary>
	int _index;
	/// <summary>
	/// moves every element in the idlist one index close to the end of the list
	/// </summary>
	void move();
};
#endif
