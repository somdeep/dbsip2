Database Systems Implementation 
Project 2 Part 1

Kaustubh Upadhyay - ku2151
Somdeep Dey - sd2988


Implementation Aspects

1. Pointer-Free Tree Structure
	An array of pointers to arrays for each level is maintained. The tree structure itself contains no pointers. 
	Building of the index is done in the following manner : 
	---> generate_sorted_unique is used to obtain unique values
	---> insertion is always begun from the last level
	---> nodesize is checked for each level, if less than (fanout-1){which is the allowed number of nodes for that level} for that level,insertion is performed, else we keep moving up a level
	---> value pointer is made to point to the pointer stored in levelArray for that level, and incremented by the current pointer for that level,which is itself incremented later

	---> a check is made after insertion if the root contains INT32_MAX, indicating it is empty, message returned appropriately.


2. Probing 
	---> A binary search module is implemented that takes in a pointer to int32_t integer, the array to search in is sent here.
	--->  start and end store the range to search for in binarysearch
	---> traversed is an indicator of the indices that should be bypassed at that particular level to access the current relevant index in a level array.
	---> Count array stores the range calculated for each probe value. this is done in the following manner:
		Now at any node, to find its range, we need the number of pages already crossed(everything lesser than it in the left tree) and this 
		is added to the part(index within searched node).
	---> Finally the count(range identifier) for each node is displayed from the array of results.