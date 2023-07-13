#ifndef CGI_HPP
# define CGI_HPP

# include "../Utilities/Utilities.hpp"
# include <unistd.h>
# include "DataContainers.hpp"

class CGI
{
    public:
        CGI();
        ~CGI();

        string exec_cgi(string file_path);
    private:
};

#endif
