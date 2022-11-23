#include <iostream>
#include "ClientManager.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;

int main() {
    try {
        ClientManager* clientManager = new ClientManager();
        clientManager->handleRequests();
        delete(clientManager);

        return 0;
    }

    catch (const std::exception& e) {
        int errorCode = GetLastError();
        std::cout << e.what() << std::endl;
        return errorCode;
    }
    
}