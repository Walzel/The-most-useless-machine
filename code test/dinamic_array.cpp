#include <iostream>  // Include standard input-output library

int test_array[6];



void print_array(int printed_array[6]){
    std::cout << "Array elements: ";
    for (int i = 0; i < 6; i++) {
        std::cout << printed_array[i] << " ";
    }
    std::cout << std::endl;

}

void add_element(int new_element, int edited_array[6]){
    for (int i=0; i < 6; i++){
        if (edited_array[i] == 0){
            edited_array[i] = new_element;
            break;
        }
    }
}

void remove_element(){
    int new_array[6];
    for (int i = 1; i < 6; i++)
    {
        new_array[i-1]=test_array[i];
    }  
    new_array[5]=0;

    for (int i = 0; i < 6; i++)
    {
        test_array[i]=new_array[i];
    }

}


int main() {
    int num;
    while (true)
    {
        std::cout << "Enter an action (0 to delete and 1-6 to add and 9 to end): ";
        std::cin >> num;
        if (num==0)
        {
            std::cout << "Deleted element";
            remove_element();
            print_array(test_array);
        }
        else if (num==9)
        {
            return 0;
        }
        
        else
        {
            add_element(num,test_array);
            std::cout << "Added element";
            print_array(test_array);
        }
        

    }
    return 0;  // Return 0 indicates successful execution
}
