#pragma once
#ifndef GLOBAL_THINGS_H
#define GLOBAL_THINGS_H


#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

static bool setValueOnce = true;
static std::unordered_map<int, std::vector<int>> tradeConditonSubsets;
	

// Function to find all subsets of given set. Any repeated subset is considered only once in the output
static void calculateAllSubsets()
{
	//std::vector<int> nums{ 1, 2, 4, 8, 16 , 32, 64 , 128 , 256, 512, 1024, 2048, 4096, 8192, 16384 };
	std::vector<int> nums{ 1, 2, 4, 8, 16, 128, 8192, 16384 }; 
	int bits = nums.size();	 // size of array to set bit
	unsigned int pow_set_size = pow(2, bits);	 // total number of subsets = pow(2, sizeof(arr))
	//sort(nums.begin(), nums.end());	 // sort to avoid adding permutation of the substring

	std::vector<std::string> list;	 // to store subset as a list to avoid adding exact duplicates

	int sum = 0;

	// counter 000..0 to 111..1
	for (int counter = 0; counter < pow_set_size; counter++) {
		std::vector<int> subset;
		std::string temp = "";
		sum = 0;
		// check for the current bit in the counter
		for (int j = 0; j < bits; j++) {
			if (counter & (1 << j)) {
				subset.push_back(nums[j]);
				// add special character to separate integers
				temp += std::to_string(nums[j]) + '$';
				sum += nums[j];
			}
		}

		if (find(list.begin(), list.end(), temp) == list.end()) {
			if (sum != 0)
				tradeConditonSubsets[sum] = subset;
			list.push_back(temp);
		}
	}

}



#endif // !GLOBAL_THINGS_H

/*
Trade condition set
{ 1, 2, 4, 8, 16 , 32, 64 , 128 , 256, 512, 1024, 2048, 4096, 8192, 16384 }
{ U  R  AX AY AJ   AW  k    BD    a    BB    BC   SA     TC   AU    AV }

Here in MCX condition available: {1,2,4,8,16,128,8192,16384} i.e. {U R AX AY AJ BD AU AV}
*/

/* Example of subsets
Map Key: Values ( Map key is sum of its values)
1 :1
2 :2
3 :1 2
4 :4
5 :1 4
6 :2 4
7 :1 2 4
8 :8
9 :1 8
10 :2 8
11 :1 2 8
12 :4 8
13 :1 4 8
14 :2 4 8
15 :1 2 4 8




*/