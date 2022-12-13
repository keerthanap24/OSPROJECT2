#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 10
#define NUM_CUSTOMERS 10
#define NUM_COOKS 2
#define ORDERS_PER_CUSTOMER 1
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

void* BENSCHILLIBOWLCustomer(void* tid) {
	int customer_id = (int)(long) tid;
  for (int i = 0; i < ORDERS_PER_CUSTOMER; i++){
      Order *order = (Order*) malloc(sizeof(Order));
      MenuItem menu_item = PickRandomMenuItem();
      order->customer_id = customer_id;
			order->menu_item = menu_item;
      order->next = NULL;
      int order_number = AddOrder(bcb, order);
      printf("Customer #%d added order #%d\n", customer_id, order_number);
    }
    return NULL;
}

void* BENSCHILLIBOWLCook(void* tid) {
  int cook_id = (int)(long) tid;
  int orders_fulfilled = 0;
  Order *order;
  order = GetOrder(bcb);
  
    // get orders from the restaurants until there's no order available.
  while(order != NULL){
      free(order); 
      orders_fulfilled += 1;
      order = GetOrder(bcb);
    }
    printf("Cook #%d fulfilled %d orders\n", cook_id, orders_fulfilled);
    return NULL;
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
  bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);
  pthread_t customers[NUM_CUSTOMERS];
  pthread_t cooks[NUM_COOKS];
  int customers_ids[NUM_CUSTOMERS];
  int cook_ids[NUM_COOKS];
  int i;
  
  for(i =0;i < NUM_COOKS; i++){
    cook_ids[i]=i+1;
    pthread_create(&cooks[i], NULL, &BENSCHILLIBOWLCook, (void *) &(cook_ids[i]));
  }
  
  for(i =0;i < NUM_CUSTOMERS; i++){
    customers_ids[i]=i+1;
    pthread_create(&customers[i], NULL, &BENSCHILLIBOWLCustomer, (void *) &(customers_ids[i]));
  }
                   
  for(i =0;i < NUM_CUSTOMERS; i++){
   pthread_join(customers[i], NULL);
  }
  for(i =0;i < NUM_COOKS; i++){
   pthread_join(cooks[i], NULL);
  }
  CloseRestaurant(bcb);
}