#include <string>
using std::string;

class myBase64 {
    private:
        static char DC[64];
        static char digit2Char(int n);
        static int char2Digit(char c);

    public:
        static string encode(const string & s);
        static string decode(const string & s);
};