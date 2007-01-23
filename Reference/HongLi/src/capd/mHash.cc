#include "mHash.hh"
#include <math.h>

double mHash::A = (sqrt(5)-1.0)/2.0;

mHash::mHash(int tablesize, int slotsize, float x, float y, float z, float sx,
float sy, float sz)
{
int i, j;
HASHTABLE * cur;
	tableSize = tablesize;
	slotSize = slotsize;
	dx = x;
	dy = y;
	dz = z;
	xo = sx; 
	yo = sy;
	zo = sz;
	

	slots = (HASHTABLE **) new (HASHTABLE *) [tableSize];
	assert(slots != NULL) ;

	for (i = 0; i < tableSize; i++) 
		{
		slots[i] =  (HASHTABLE *) new HASHTABLE[slotSize];
		assert(slots[i] != NULL) ;
		cur = slots[i];	
		for(j = 0; j < slotSize; j++)	
			{
			cur->pV = NULL;
//			cur->index = -1;
			cur ++;
			}
		}		
}

mHash::~mHash()
{
int i;
for(i = 0; i <tableSize; i++)
	delete slots[i];
delete slots;
}


Vertex_with_Features * mHash::HashCompare(Vertex_with_Features * Vin)
{
int j;
double k;
int num;
Vertex_with_Features * newV, *curV;
HASHTABLE * cur;

	k = Vin->getPX()*dx + Vin->getPY()*dy + Vin->getPZ()*dz + xo + yo + zo;
	k = k*A;
	num = (int)floor(((double)tableSize*(k - floor(k))));
//	cout << num <<endl;
	cur = slots[num];
	for (j = 0 ;j<slotSize; j++)
		{
		if(cur->pV == NULL)
			{
			//newV = new Vertex_with_Features(Vin);
			//assert(newV != NULL);
			return cur->pV = Vin;
			}
		else {
			curV = cur->pV;
			//if(curV == Vin)
			//	{
			//	return curV;
			//	}
			if(*curV == *Vin)
				{
			//	delete Vin;
				return curV;
				}	
			}	
		cur ++;	
		}			
	assert(j != slotSize); //overflow 
	return NULL;	
}

