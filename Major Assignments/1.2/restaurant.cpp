#include "restaurant.h"
/*
	Sets *ptr to the i'th restaurant. If this restaurant is already in the cache,
	it just copies it directly from the cache to *ptr. Otherwise, it fetches
	the block containing the i'th restaurant and stores it in the cache before
	setting *ptr to it.
*/
void getRestaurant(restaurant* ptr, int i, Sd2Card* card, RestCache* cache) {
	// calculate the block with the i'th restaurant
	uint32_t block = REST_START_BLOCK + i/8;

	// if this is not the cached block, read the block from the card
	if (block != cache->cachedBlock) {
		while (!card->readBlock(block, (uint8_t*) cache->block)) {
			Serial.print("readblock failed, try again");
		}
		cache->cachedBlock = block;
	}

	// either way, we have the correct block so just get the restaurant
	*ptr = cache->block[i%8];
}

// Swaps the two restaurants (which is why they are pass by reference).
void swap(RestDist& r1, RestDist& r2) {
	RestDist tmp = r1;
	r1 = r2;
	r2 = tmp;
}

// Insertion sort to sort the restaurants.
void insertionSort(RestDist restaurants[], int rated_restaurants_num) {
	// Invariant: at the start of iteration i, the
	// array restaurants[0 .. i-1] is sorted.
	uint32_t start_time = millis();
	for (int i = 1; i < rated_restaurants_num; ++i) {
		// Swap restaurant[i] back through the sorted list restaurants[0 .. i-1]
		// until it finds its place.
		for (int j = i; j > 0 && restaurants[j].dist < restaurants[j-1].dist; --j) {
			swap(restaurants[j-1], restaurants[j]);
		}
	}
	uint32_t end_time = millis()-start_time;
	Serial.print("isort ");Serial.print(rated_restaurants_num);
	Serial.print(" restaurants: ");Serial.print(end_time);Serial.println(" ms");

}

int32_t partition(RestDist restaurants[], int32_t left_index, int32_t right_index, int32_t pivot){
    while (left_index <= right_index){
        // keep looping if left is smaller than right index
        while (restaurants[left_index].dist < pivot){
            // keep moving left index right until value is bigger than pivot
            left_index++;
        }
        while (restaurants[right_index].dist > pivot){
            // keep moving right index left unril value is less than pivot
            right_index--;
        }
        if (left_index <= right_index){
            // swap the two values so that small is on left and big is on right of pivot
            swap(restaurants[left_index], restaurants[right_index]);
            // increment left and right indexes
            left_index++;
            right_index--;
        }
    }
    // return the left index
    return left_index;
}
int quickSort(RestDist restaurants[], int32_t left_index, int32_t right_index){

    if (left_index < right_index){
        // pivot chosen to be center of array
        int pivot = restaurants[(left_index+right_index)/2].dist;
        // call partition to get the pivot index
        uint32_t pivot_index = partition(restaurants, left_index, right_index, pivot);
        //recursively call twice
        // recursively sort array before and after pivot
        quickSort(restaurants, left_index, pivot_index-1);
        quickSort(restaurants, pivot_index, right_index);
    }
}

// Computes the manhattan distance between two points (x1, y1) and (x2, y2).
int16_t manhattan(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	return abs(x1-x2) + abs(y1-y2);
}

/*
	Fetches all restaurants from the card, saves their RestDist information
	in restaurants[], and then sorts them based on their distance to the
	point on the map represented by the MapView.
*/
void getAndSortRestaurants(const MapView& mv, RestDist restaurants[], Sd2Card* card, RestCache* cache, int sort_mode, int rating) {
	restaurant r;

	// First get all the restaurants and store their corresponding RestDist information.
	int array_index = 0;
	for (int i = 0; i < NUM_RESTAURANTS; ++i) {
		getRestaurant(&r, i, card, cache);
		if (r.rating >= rating){
			// only sort restaurants over the rating threshold
			// store the index of restaurant on the SD card and the manhattan distance
			restaurants[array_index].index = i;
			restaurants[array_index].dist = manhattan(lat_to_y(r.lat), lon_to_x(r.lon),
							 				mv.mapY + mv.cursorY, mv.mapX + mv.cursorX);
			array_index++; //increment array index
		}
	}

	if (sort_mode == 0){
		// if 0 is quick sort
		uint32_t start_time = millis();
		quickSort(restaurants,0,array_index-1);
		uint32_t end_time = millis()-start_time;
		Serial.print("qsort ");Serial.print(array_index);
    	Serial.print(" restaurants: ");Serial.print(end_time);Serial.println(" ms");
    	Serial.flush();
	}else if(sort_mode == 1){
		// if 1 is insertion sort
		insertionSort(restaurants , array_index);
	}else if(sort_mode == 2){
		// if 2 is both
		uint32_t start_time = millis();
		quickSort(restaurants,0,array_index-1);
		uint32_t end_time = millis()-start_time;
		Serial.print("qsort ");Serial.print(array_index);
    	Serial.print(" restaurants: ");Serial.print(end_time);Serial.println(" ms");
    	
    	// get the unsorted restaurants again
    	int array_index = 0;
		for (int i = 0; i < NUM_RESTAURANTS; ++i) {
			getRestaurant(&r, i, card, cache);
		if (r.rating >= rating){
			// only sort restaurants over the rating threshold
			// store the index of restaurant on the SD card and the manhattan distance
			restaurants[array_index].index = i;
			restaurants[array_index].dist = manhattan(lat_to_y(r.lat), lon_to_x(r.lon),
							 				mv.mapY + mv.cursorY, mv.mapX + mv.cursorX);
			array_index++; //increment array index
			}
		}
		insertionSort(restaurants , array_index);
	}
	Serial.flush();
}
