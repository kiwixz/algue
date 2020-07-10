#include <kae/exception.h>
#include <kae/os.h>

int main(int /*argc*/, char** /*argv*/)
{
    std::set_terminate(&kae::terminate);
    kae::set_thread_name("main_thread");
}
