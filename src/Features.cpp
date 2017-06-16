/**
 * @Class Features
 * @details Preprocessing and calculation of face features.
 */
#include "Features.h"

/**
 * @brief Empty constructor
 * @details Initializes the class
 */
Features::Features()
{
	size = -1;
	label = -1;
	data = NULL;
}

/**
 * @brief Copy constructor
 */
Features::Features(const Features& f)
{
	this->size = f.size;
	this->label = f.label;

	this->data = new float[size];
	for(int i = 0; i < size; i++)
		this->data[i] = f.data[i];
}

/**
 * @brief Copy constructor
 */
Features::Features(Features* f)
{
	this->size = f->size;
	this->label = f->label;

	this->data = new float[size];
	for(int i = 0; i < size; i++)
		this->data[i] = f->data[i];
}

/**
 * @brief Destructor
 */
Features::~Features()
{
	if(data != NULL)
		delete [] data;
}

/**
 * @brief returns the ith Feature of the array
 * @param i(int) Feature index
 * @return return the ith feature
 */
int Features::operator[] (const int& i)
{
	if(size < 1 || i >= size)
		return -1;
	else
		return data[i];
}

/**
 * @brief Concatenate a new feature with the existing features
 * @param F(Features) Feature to be concatenated
 * @return void
 */
void Features::featCat(Features& F)
{
	int i;
	float* aux = new float[size];

	for(i = 0; i < size; i++)
		aux[i] = data[i];

	delete [] data;

	data = new float[this->size+F.size];

	for(i = 0; i < size; i++)
		data[i] = aux[i];

	delete [] aux;

	for(i = size; i < size + F.size; i++)
		data[i] = F.data[i-size];

	this->size+=F.size;
}

/**
 * @brief Sum a new feature with the existing features
 * @param F(Features) Feature to be summed
 * @return void
 */
void Features::featSum(Features& F)
{
	for(int i = 0; i < size; i++)
		data[i] += F.data[i];
}