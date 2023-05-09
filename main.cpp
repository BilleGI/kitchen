#include <iostream>
#include <ctime>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>

int allDelivery = 0;
const int DELIVERY(10);
const int TIMECOOKING(30);

std::queue<std::string> order;
std::queue<std::string> cooking;

std::mutex cookAccess;
std::mutex orderAccess;
std::mutex deliveryAccess;
std::mutex exetAccess;

std::string generateOrder()
{
    std::string order;
    int numOrder = rand()%5;

    switch(numOrder)
    {
    case 0:
        order = "pizza";
        break;
    case 1:
        order = "soup";
        break;
    case 2:
        order = "steak";
        break;
    case 3:
        order = "salad";
        break;
    case 4:
        order = "sushi";
        break;
    }

    return order;
}

void deliveryDish()
{

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(TIMECOOKING));
        std::cout << "The deliveryman has arrived!" << std::endl;
        deliveryAccess.lock();
        std::cout << cooking.size() << std::endl;
        std::cout << "In the delivery of " << allDelivery + 1 << " dish: ";
        for (int i = 0; !cooking.empty(); ++i)
        {
            if(cooking.size() == 1) std::cout << cooking.front() << "." << std::endl;
            else std::cout << cooking.front() << ", ";
            cooking.pop();
        }
        deliveryAccess.unlock();

        exetAccess.lock();
        ++allDelivery;
        if(allDelivery >= DELIVERY)
        {
            exetAccess.unlock();
            return;
        }
        exetAccess.unlock();
    }
}

void cookDish(int num)
{
    cookAccess.lock();

    std::cout << "\nNum: " << num  << std::endl;
    std::cout << "The " << order.front() << " dish has started to cook." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds((std::rand()%6)+10));
    orderAccess.lock();
    deliveryAccess.lock();
    cooking.push(order.front()); // взяли первый элемент
    deliveryAccess.unlock();
    order.pop(); // удалили его
    orderAccess.unlock();
    std::cout << "Dish " << cooking.back() << " is cooked." << std::endl;
    cookAccess.unlock();
}

int main()
{
    int numOrder = 0;

    std::cout << "The program simulates the operation of the kitchen." << std::endl;

    std::thread delivery(deliveryDish);
    std::srand(std::time(nullptr));
    while(true)
    {
        if (allDelivery < DELIVERY)
        {
            orderAccess.lock();
            order.push(generateOrder());
            orderAccess.unlock();
            std::this_thread::sleep_for(std::chrono::seconds((std::rand()%6)+5));
            for (int var= 0; var < 40; ++var) {
                std::cout << "\\";
            }
            std::cout << "\nOrder " << numOrder + 1 << " dish: " << order.back() << " delivered to the kitchen." << std::endl;


            std::thread cookingThread(cookDish, numOrder + 1);
            cookingThread.detach();
            ++ numOrder;
        }

        exetAccess.lock();
            if(allDelivery >= DELIVERY)
            break;
        exetAccess.unlock();

    }
    delivery.join();
    return 0;
}
