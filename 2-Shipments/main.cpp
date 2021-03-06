//
//  main.cpp
//  2-Shipments
//
//  Created by Moshe Berman on 11/25/12.
//  Copyright (c) 2012 Moshe Berman. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>

#include "Warehouse.h"
#include "Card.h"

//  Some utility methods I wrote for file IO.
#include "FileIO/FileIO.h"

using namespace std;

//
//  Method implementations are preceeded by
//  full explanations of each piece of code.
//

bool pricesFromStringToArray(string input, double *);
string cityNameFromCard(string);
string pricesForWarehouse(string card);
bool loadQuantitiesFromString(string, int[], const int);
Warehouse *warehouseForNameFromArray(string , Warehouse [], int);
bool warehouseHasDesiredAmountOfItem(Warehouse, int, int);
Warehouse *warehouseMostStockThatFillsItemAndAmount(Warehouse[], int, int, int);

//
//  Declare a constant for the number of variables
//

const int numberOfWarehouses = 5;

//
//  The names of the warehouses
//

string warehouseNames[numberOfWarehouses] = {"New York", "Los Angeles", "Miami", "Houston", "Chicago"};

int main(int argc, const char * argv[])
{

    //
    //  Since we know there are 5 warehouses,
    //  we can declare a static array. No need
    //  for dynamic arrays or vectors or whatever.
    //

    Warehouse warehouses[numberOfWarehouses];
    
    //
    //  Read the warehouse names into the warehouse array.
    //  TODO: It would be awesome if we could do this dynamically,
    //  but there are too many edge cases to handle, so it's
    //  out of the scope the assignment...
    //
    
    for (int i=0; i<numberOfWarehouses; i++) {
        warehouses[i].name = warehouseNames[i];
        
        //Also prep the warehouse with default values.
        warehouses[i].quantities[0] = 0;
        warehouses[i].quantities[1] = 0;
        warehouses[i].quantities[2] = 0;
    }
    
    //
    //  Open an the records file
    //
    
    ifstream recordsFile;
    string fileName = "cards.txt";
    
    //  If opening fails, print out an error.
    
    if(!openInputStream(recordsFile, fileName)){
        cout << "Could not open " << fileName << endl;
    }
    
    //  Otherwise, proceed to parse the file
    
    else{
      
        //
        //  Declare some variables to hold the prices & quantities
        //
        
        double prices[3];
        int quantities[3];
        
        //
        //  Read out the line that contains the
        //  prices and parse it into an array.
        //
        
        string pricesString;
    
        getline(recordsFile, pricesString);
        
        pricesFromStringToArray(pricesString, prices);  
        

        //
        //  Read in each line and process it.
        //
    
        while (!recordsFile.eof()) {
        
        string cardString;
        
        getline(recordsFile, cardString);
        
        //
        //  Check for an empty line
        //
        
        if (cardString != "") {
         
            //
            //  Process the card
            //
            
            Card card;
            
            card.cardType = cardString[0];
            card.city = cityNameFromCard(cardString);
            
            if(!loadQuantitiesFromString(pricesForWarehouse(cardString), quantities, 3)){
                cout << "Failed to read quantities from the card. Skipping this card." << endl;
                continue;
            }
            
            //
            //  Put the quantities in the card.
            //
            
            card.amount1 = quantities[0];
            card.amount2 = quantities[1];
            card.amount3 = quantities[2];
            
            //
            //  Choose the correct warehouse
            //
            
            Warehouse &workingWarehouse = *warehouseForNameFromArray(card.city, warehouses, numberOfWarehouses);
            
            //  Print out the card, since we want to do that
            //  regardless of the contents of the card.
            
            cout << card.description();
            
            //
            //  Handle shipments here...
            //
            
            if (card.cardType == 's') {
                
                //  ...update the quantities...
                
                workingWarehouse.quantities[0] += card.amount1;
                workingWarehouse.quantities[1] += card.amount2;
                workingWarehouse.quantities[2] += card.amount3;
                
                //  ..and print out the warehouse values.
                
                cout << workingWarehouse.description();
                
            }
            
            //  ... and Orders here.
            
            else if(card.cardType == 'o'){
             
                // First print out the card
                
                double price = 0;
                bool orderFilled = true;
                
                //
                //  Process each of the desired items...
                //
                
                for(int i=0; i <3; i++){
                    
                    //
                    //  If the item is available, process the order...
                    //
                    
                    if (warehouseHasDesiredAmountOfItem(workingWarehouse, quantities[i], i)) {
                        
                        //  Add the item to the total cost, and
                        //  subtract from the total available.
                        
                        price += quantities[i] * prices[i];
                        workingWarehouse.quantities[i] -= quantities[i];
                        
                    }
                    
                    //
                    //  ... otherwise the item is not available in the requested warehouse. 
                    //
                    
                    else{
                    
                        Warehouse *alternateWarehouse = warehouseMostStockThatFillsItemAndAmount(warehouses, numberOfWarehouses, i, quantities[i]);
                        
                        //
                        //  Because we might get an insufficient warehouse
                        //  if there are none, we need to double check here.
                        //
                        
                        bool alternateWarehouseIsSufficient = warehouseHasDesiredAmountOfItem((*alternateWarehouse), quantities[i], i);
                        
                        //
                        //  If there's no alternate warehouse, flag
                        //  the order as unfillable.
                        //
                        
                        if (alternateWarehouse == NULL || !alternateWarehouseIsSufficient) {
                            orderFilled = false;
                        }
                        
                        //
                        //  If there's an alternate warehouse,
                        //  then we should process from there.
                        //
                        
                        else{
                        
                            //  Charge an extra 10% and deduct from the
                            //  correct warehouse.
                            price += quantities[i] * (prices[i] * 1.1);
                            (*alternateWarehouse).quantities[i] -= quantities[i];
                            
                            //
                            //  Print out that the item has been shifted.
                            //
                            
                            cout << quantities[i] << " of item " << i+1 << " shipped from ";
                            cout << (*alternateWarehouse).name << " to " << workingWarehouse.name << "." << endl;
                            
                            //
                            //  Print out the description of the alternate warehouse.
                            //
                            
                            cout << (*alternateWarehouse).description();
                        }
                        
                        
                    }
                }
        
                cout << workingWarehouse.description();
                

                cout << "Price: $" << price << endl;
                
                if(!orderFilled){
                    cout << "Order Unfulfilled" << endl;
                }
            
              }
            
        }
    }
    
    }
    return 0;
}

//
//  Parses a string which contains dollar amounts
//  into an array of double values.
//
//  For the code to work, we assume that prices are
//  embedded in a single line string, preceeded by
//  a dollar sign, and followed by a space. If the format
//  varies, this code won't work.
//
//  IMPORTANT: We don't resize the buffer. So you have
//  to know how many strings you're planning to extract
//  ahead of time.
//

bool pricesFromStringToArray(string input, double *buffer){
    
    size_t position = 0;
    
    int indexForBuffer = 0;
    
    while (position < input.length()-1) {
        
        size_t positionOfDollarSign = input.find("$", position);
        
        //
        //  If there are no dollar signs, just
        //  set the position to out of range
        //  of the while loop and it will exit.
        //
        
        if (positionOfDollarSign == string::npos) {
            position = input.length();
            break;
        }
        
        size_t positionOfTrailingSpace = input.find(" ", positionOfDollarSign-1);
        
        //
        //  We found the last dollar sign,
        //  so take the substring from the
        //  dollar sign until the end.
        //
        
        if(positionOfTrailingSpace == string::npos){
            
            //
            //  Get the index of the last character
            //
            
            positionOfTrailingSpace = input.length()-1;
        }
        
        
        //
        //  By this point, we have to indices to use
        //  to grab a substring from the input.
        //
        
        string priceValueAsString;
        
        priceValueAsString = input.substr(positionOfDollarSign+1, positionOfTrailingSpace-positionOfDollarSign);
        
        //
        //  Convert the string into a double via a stream.
        //
        //  If for some reason the conversion fails, then
        //  return false
        //
        
        istringstream stringToDoubleConverter(priceValueAsString);
        
        if(!(stringToDoubleConverter >> buffer[indexForBuffer])){
            return false;
        }
        
        //
        //  Move the "header" of the loop to
        //  the next part of the string.
        //
        
        position = positionOfTrailingSpace;
        
        //
        //  Don't forget to update the buffer index
        //
        
        indexForBuffer++;
    }
    
    return true;
}

//
//  This method searches for
//  known city names and returns
//  the one that occurs in the
//  shipment card.
//
//  Known city names are defined in the
//  global array "warehouseNames".
//
//  Note that this is necessary because
//  city names have spaces.
//

string cityNameFromCard(string card){
    for (int i = 0; i < numberOfWarehouses; i++) {
        
        if (card.find(warehouseNames[i]) != string::npos) {
            return warehouseNames[i];
        }
    }
    
    return "";
}

//
//  This method returns the contents
//  of a card which follow its city name.
//
//  Based on our format, this is the prices.
//

string pricesForWarehouse(string card){
    
    string warehouseName = cityNameFromCard(card);
    
    size_t locationOfString = card.find(warehouseName);
    
    return card.substr(locationOfString+warehouseName.length());
}

//
//  Takes a string with prices, an array, and
//  the number of quantities we expect to load.
//

bool loadQuantitiesFromString(string pricesString, int quantities[], const int numberOfQuantities){
    
    istringstream conversionStream(pricesString);
    
    for (int i = 0; i < numberOfQuantities; i++) {
        if(!(conversionStream >> quantities[i])){
            return false;
        }
    }

    return true;
}

//
//  Returns a warehouse from a given
//  array, matching a given name.
//

Warehouse *warehouseForNameFromArray(string name, Warehouse warehouses[], int numberOfWarehouses){
    
    Warehouse *workingWarehouse = NULL;
    
    for (int i = 0; i<numberOfWarehouses; i++) {
        if (warehouses[i].name == name) {
            workingWarehouse = &warehouses[i];
        }
    }
    
    return workingWarehouse;
}

//
//  Checks if a given warehouse has enough stock of a given item
//

bool warehouseHasDesiredAmountOfItem(Warehouse warehouse, int desiredAmount, int item){
    return warehouse.quantities[item] >= desiredAmount;
    
}

//
//  Finds a warehouse from an array of warehouses
//  that can handle the requested item in the
//  quantity that was requested.
//

Warehouse *warehouseMostStockThatFillsItemAndAmount(Warehouse warehouses[], int numberOfWarehouses, int desiredAmount, int item){
    
    Warehouse *workingWarehouse = NULL;
    
    //
    //  For each warehouse, check if it
    //  has enough and more than the last
    //  warehouse we checked.
    //
    
    for (int i = 0; i<numberOfWarehouses; i++) {
        if (warehouses[i].quantities[item] >= desiredAmount) {
            if (workingWarehouse == NULL){
                workingWarehouse = &warehouses[i];
            }
            else if(warehouses[i].quantities[item] > (*workingWarehouse).quantities[item]){
                workingWarehouse = &warehouses[i];
            }
        }
    }
        
    return workingWarehouse;
}

