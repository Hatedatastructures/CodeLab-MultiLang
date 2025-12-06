#include <iostream>
extern "C" 
{
    void cfunc(char* ip, int port);
}

namespace Go
{
    class socket
    {
        public:
            socket()
            {

            }
            ~socket()
            {
                
            }
            int connect(char* ip, int port)
            {
                std::cout << "connect" << std::endl;
                return 0;
            }
    };
}

void cfunc(char* ip, int port)
{
    Go::socket s;
    s.connect(ip, port);
}