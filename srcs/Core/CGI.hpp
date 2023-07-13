#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include "../Utilities/Utilities.hpp"
# include <unistd.h>
# include "DataContainers.hpp"

class CGI
{
    public:
        CGI();
        ~CGI();

        string exec_cgi(string file_path, string root);
    private:
};

#endif