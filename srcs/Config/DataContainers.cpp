#include "DataContainers.hpp"
#include <stdexcept>
#include "sstream"
#include <iostream>
// USE THIs AS REFERENCE FOR DEFAULTS

Data::ErrorPages::ErrorPages(): error_404("404.html"), error_502("502.html") {}

Data::ErrorPages::ErrorPages(const Data::ErrorPages &rhs): error_404(rhs.error_404), error_502(rhs.error_502) {}

Data::ErrorPages::~ErrorPages() {}

Data::Server::Server(): ports(), names(), ip(), accepted_methods(), root("www/"), index("index.html"), max_body_size(1000), errors(), locations()
{
    this->accepted_methods.push_back("get");
}

Data::Server::Server(const Data::Server &rhs): ports(rhs.ports),
names(rhs.names),
ip(rhs.ip),
accepted_methods(rhs.accepted_methods),
root(rhs.root),
index(rhs.index),
max_body_size(rhs.max_body_size),
errors(rhs.errors),
locations(rhs.locations)
{
}

Data::Conf::~Conf() {
    std::cout << "Parent Data destructed\n";
}

Data::Server::~Server() {
    std::cout << "Destroying server data\n";
}

Data::Location::Location():
        route(), root(), directory_file(), index(),
        directory_listing(false), accepted_methods(), redirection()
{}

Data::Location::Location(Data::Server const &context):
		route(), root(context.root), directory_file(), index(context.index),
		directory_listing(false), accepted_methods(context.accepted_methods), redirection()
{}

Data::Location::Location(Location const &rhs):
		route(rhs.route), root(rhs.root), directory_file(rhs.directory_file), index(rhs.index),
		directory_listing(rhs.directory_listing), accepted_methods(rhs.accepted_methods), redirection(rhs.redirection)
{}

Data::Location::~Location() { std::cout << "Destroying Location\n"; }

Data::Line::Line(Line const &rhs): key(rhs.key), raw(rhs.raw), pre(rhs.pre), tokens(rhs.tokens), n(rhs.n) {}

Data::Line::Line() {

}

Data::Line::Line(const std::string &raw, const std::string &pre, size_t n): raw(raw), pre(pre), n(n) {}

Data::Line::~Line() {

}

void Data::Line::update(const std::string &new_line)
{
   std::swap(this->pre, this->raw);
   this->raw = new_line;
   this->n++;
}

Data::Line &Data::Line::operator=(const Data::Line &rhs) {
	this->n = rhs.n;
	this->tokens = rhs.tokens;
	this->raw = rhs.raw;
    this->pre = rhs.pre;
	this->key = rhs.key;
	return *this;
}

void Data::Line::tokenize()
{
    std::stringstream	line(raw);
    std::string 	    tmp;

    this->tokens.clear();
    this->key.clear();

    while (line >> tmp)
    {
        if (this->tokens.empty() && this->key.empty())
            this->key = tmp;
        else
            this->tokens.push_back(tmp);
    }
}

std::ostream &operator<<(std::ostream &os, const Data::Line &line)
{
    std::stringstream ss;

    ss << "current -> " << line.raw << " " << "pre -> " << line.pre << " n: " << line.n;
    os << ss.str();
    return os;
}
