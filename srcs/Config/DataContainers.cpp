#include "DataContainers.hpp"

// USE THIs AS REFERENCE FOR DEFAULTS

Data::ErrorPages::ErrorPages(): error_404("404.html"), error_502("502.html") {}

Data::ErrorPages::ErrorPages(const Data::ErrorPages &rhs): error_404(rhs.error_404), error_502(rhs.error_502) {}

Data::ErrorPages::~ErrorPages() {}

Data::Server::Server(): ports(), names(), ip(), accepted_methods(), root("www/"), index("index.html"), max_body_size(1000), errors(), locations() {}

Data::Server::Server(const Data::Server &rhs): ports(rhs.ports),
names(rhs.names),
ip(rhs.ip),
accepted_methods(rhs.accepted_methods),
root(rhs.root),
index(rhs.index),
max_body_size(rhs.max_body_size),
errors(rhs.errors),
locations(rhs.locations)
{}

Data::Conf::~Conf() {

}

Data::Server::~Server() {}

Data::Location::Location() { throw (std::invalid_argument("can't construct location without server context")); }

Data::Location::Location(Data::Server const &context):
		route(), root(context.root), directory_file(), index(context.index),
		directory_listing(false), accepted_methods(context.accepted_methods), redirection()
{}

Data::Location::Location(Location const &rhs):
		route(rhs.route), root(rhs.root), directory_file(rhs.directory_file), index(rhs.index),
		directory_listing(rhs.directory_listing), accepted_methods(rhs.accepted_methods), redirection(rhs.redirection)
{}

Data::Location::~Location() {}

Data::Line::Line(Line const &rhs): key(rhs.key), raw(rhs.raw), tokens(rhs.tokens), n(rhs.n) {}

Data::Line::Line() {

}

Data::Line::~Line() {

}

Data::Line &Data::Line::operator=(const Data::Line &rhs) {
	this->n = rhs.n;
	this->tokens = rhs.tokens;
	this->raw = rhs.raw;
	this->key = rhs.key;
	return *this;
}
