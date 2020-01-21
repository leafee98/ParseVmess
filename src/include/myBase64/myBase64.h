#include <string>

class myBase64 {
    private:
        static char DC[64];
        static char digit2Char(int n);
        static int char2Digit(char c);

    public:
        static std::string encode(const std::string & s);
        static std::string decode(const std::string & s);
};