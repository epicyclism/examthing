//
// Derived from Boost Beast examples copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
//
#pragma once
#if defined (_MSC_VER)
#include "targetver.h"
#endif

#include <string>
#include <string_view>
#include <optional>
#include <memory>
#include <iostream>

using namespace std::string_view_literals;

#define BOOST_BEAST_USE_STD_STRING_VIEW
#include <boost/asio.hpp>

namespace net = boost::asio; 
using tcp = boost::asio::ip::tcp; 

#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

#include <boost/optional/optional_io.hpp>

//#include "sv_util.h"
#include "etw_version.h"

#define NOISY_SERVICE 1

bool iequals(std::string_view lhs, std::string_view rhs);
using range_data = std::optional<std::pair<int64_t, int64_t>>;
range_data decode_range(std::string_view hdr) ;
std::string make_content_range(range_data const rd, uint64_t sz);
int64_t get_content_len(range_data const& rd);

// Return a reasonable mime type based on the extension of a file.
std::string_view mime_type(std::string_view path);

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string path_cat(std::string_view base, std::string_view path);
void percent_decode(std::string& path);


template<class Body, class Allocator>
void dump_request(http::request<Body, http::basic_fields<Allocator>>const& req)
{
    if (req.method() == http::verb::get)
        std::cout << "GET\n";
    else
    if (req.method() == http::verb::post)
        std::cout << "POST\n";
    else
    if (req.method() != http::verb::head)
        std::cout << "HEAD\n";
    else
        std::cout << "UNKNOWN\n";
    std::cout << req.target() << '\n';

    for (auto& f : req)
    {
        std::cout << f.name_string() << " : " << f.value() << '\n';
    }
    std::cout << "payload size = " << req.payload_size() << "\n";
}

template<class Body, class Allocator>
void dump_response(http::response<Body, http::basic_fields<Allocator>>const& res)
{
    std::cout << "RESPONSE: " << res.base().result() << '\n';
    for (auto& f : res)
    {
        std::cout << f.name_string() << " : " << f.value() << '\n';
    }
    std::cout << '\n';
}

template<typename WKR>
class http_session : public std::enable_shared_from_this<http_session<WKR>>
{
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    WKR  wkr_;
    std::string_view doc_root_;
    bool verbose_;

    // The parser is stored in an optional container so we can
    // construct it from scratch it at the beginning of each new message.
    std::optional<http::request_parser<http::string_body>> parser_;

    void fail(beast::error_code ec, char const* what)
    {
        // Don't report on canceled operations
        if ( ec == net::error::operation_aborted || ec == net::error::connection_aborted || ec == net::error::timed_out || ec == beast::error::timeout)
            return;
        std::cerr << what << ": " << ec.message() << "\n";
    }

    template<
        class Body, class Allocator,
        class Send>
        void
        handle_request(
            std::string_view doc_root,
            http::request<Body, http::basic_fields<Allocator>>&& req,
            Send&& send)
    {
        // Returns a bad request response
        auto const bad_request =
            [&req](std::string_view why)
        {
            http::response<http::string_body> res{ http::status::bad_request, req.version() };
            res.set(http::field::server, etw_user_agent);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = std::string(why);
            res.prepare_payload();
            return res;
        };

        // Returns a not found response
        auto const not_found =
            [&req](std::string_view target)
        {
            http::response<http::string_body> res{ http::status::not_found, req.version() };
            res.set(http::field::server, etw_user_agent);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "The resource '" + std::string(target) + "' was not found.";
            res.prepare_payload();
            return res;
        };

        // Returns a server error response
        auto const server_error =
            [&req](std::string_view what)
        {
            http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
            res.set(http::field::server, etw_user_agent);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "An error occurred: '" + std::string(what) + "'";
            res.prepare_payload();
            return res;
        };

        // Returns a JSON encoded REST GET response
        auto const rest_response =
            [&req](std::string_view json)
        {
            http::response<http::string_body> res{ http::status::ok, req.version() };
            res.set(http::field::server, etw_user_agent);
            res.set(http::field::content_type, "application/json");
            res.keep_alive(req.keep_alive());
            res.body() = json;
            res.prepare_payload();
            return res;
        };

        // returns a 'post' processed response
        auto const post_response =
            [&req](std::string_view result, std::string_view type, std::string_view disposition)
        {
            http::response<http::string_body> res{ http::status::ok, req.version() };
            res.set(http::field::server, etw_user_agent);
            res.set(http::field::content_type, type);
            if(!disposition.empty())
                res.set(http::field::content_disposition, disposition);
            res.keep_alive(req.keep_alive());
            res.body() = result;
            res.prepare_payload();
            return res;
        };

        if(verbose_)
            dump_request(req);

        // filter off the posts
        if (wkr_.can_post() && req.method() == http::verb::post)
        {
            if (wkr_.can_verb(req.target()))
            {
                auto [result, type, disposition] = wkr_.process(req.target(), req["content-Type"], req.body());
                return send(post_response(std::move(result), type, disposition));
            }
            else
                return send(not_found(req.target()));
        }

        if (!(req.method() == http::verb::get || req.method() == http::verb::head ))
            return send(bad_request("Unknown HTTP-method"));

        // Request path must be absolute and not contain "..".
        if (req.target().empty() ||
            req.target()[0] != '/' ||
            req.target().find("..") != std::string_view::npos)
            return send(bad_request("Illegal request-target"));

        // Build the path to the requested file
        std::string path = path_cat(doc_root, req.target());
        percent_decode(path);
        if (req.target().back() == '/' || req.target().back() == '\\')
            path.append("index.html");
        // Attempt to open the file
        beast::error_code ec;
        http::file_body::value_type body;
        body.open(path.c_str(), beast::file_mode::scan, ec);

        // Handle the case where the file doesn't exist
        if (ec == boost::system::errc::no_such_file_or_directory)
            return send(not_found(req.target()));

        // Handle an unknown error
        if (ec)
            return send(server_error(ec.message()));

        // Cache the size since we need it after the move
        auto const size = body.size();
        // Respond to HEAD request
        if (req.method() == http::verb::head)
        {
            http::response<http::empty_body> res{ http::status::ok, req.version() };
            res.set(http::field::server, etw_user_agent);
            res.set(http::field::content_type, mime_type(path));
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return send(std::move(res));
        }

        // Respond to GET request
        http::response<http::file_body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version()) };
        res.set(http::field::server, etw_user_agent);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        
        if(verbose_)
            dump_response(res);

        return send(std::move(res));
    }
       
    void do_read()
    {
        // Construct a new parser for each message
        parser_.emplace();

        // max data we can handle is now 5mb...
        parser_->body_limit(5*1024*1024);

        // Set the timeout.
        stream_.expires_after(std::chrono::seconds(30));
        auto self = http_session<WKR>::shared_from_this();
        // Read a request
        http::async_read(
            stream_,
            buffer_,
//            parser_->get(),
            *parser_,
            beast::bind_front_handler(
                &http_session<WKR>::on_read,
                self));
    }

    void on_read(beast::error_code ec, std::size_t)
    {
        // This means they closed the connection
        if (ec == http::error::end_of_stream)
        {
            stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
            return;
        }

        // Handle the error, if any
        if (ec)
            return fail(ec, "read");

        // Send the response
        handle_request(
            doc_root_,
            parser_->release(),
            [this](auto&& response)
            {
                // The lifetime of the message has to extend
                // for the duration of the async operation so
                // we use a shared_ptr to manage it.
                using response_type = typename std::decay<decltype(response)>::type;
                auto sp = std::make_shared<response_type>(std::forward<decltype(response)>(response));

                // Write the response
                auto self = http_session<WKR>::shared_from_this();
                http::async_write(stream_, *sp,
                    [self, sp](
                        beast::error_code ec, std::size_t bytes)
                    {
                        self->on_write(ec, bytes, sp->need_eof());
                    });
            });
    }
    void on_write(beast::error_code ec, std::size_t, bool close)
    {
        // Handle the error, if any
        if (ec)
            return fail(ec, "write");

        if (close)
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
            return;
        }

        // Read another request
        do_read();
    }

public:
    http_session( tcp::socket&& socket, std::string_view doc_root,  bool verbose) : stream_(std::move(socket)), doc_root_(doc_root), verbose_(verbose)
    {
    }

    void run()
    {
        do_read();
    }
};

// Accepts incoming connections and launches the sessions
template<typename WKR>
class http_listener : public std::enable_shared_from_this<http_listener<WKR>>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    std::string_view doc_root_;
    bool verbose_;
    // do something more interesting soon
    void fail(beast::error_code ec, char const* what)
    {
        // Don't report on canceled operations
        if (ec == net::error::operation_aborted)
            return;
        std::cerr << what << ": " << ec.message() << "\n";
    }
    void on_accept(beast::error_code ec, tcp::socket socket)
    {
        if (ec)
            return fail(ec, "accept");
        else
            // Launch a new session for this connection
            std::make_shared<http_session<WKR>>(
                std::move(socket), doc_root_, verbose_)->run();

        auto self = http_listener<WKR>::shared_from_this();
        // The new connection gets its own strand
        acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(
                &http_listener<WKR>::on_accept, self));
    }

public:
    http_listener(net::io_context& ioc, tcp::endpoint endpoint, std::string_view doc_root, bool verbose) : ioc_(ioc), acceptor_(ioc), doc_root_(doc_root), verbose_(verbose)
    {
        beast::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if (ec)
        {
            fail(ec, "open");
            return;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec)
        {
            fail(ec, "set_option");
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec)
        {
            fail(ec, "bind");
            return;
        }

        // Start listening for connections
        acceptor_.listen(
            net::socket_base::max_listen_connections, ec);
        if (ec)
        {
            fail(ec, "listen");
            return;
        }
    }

    // Start accepting incoming connections
    void run()
    {
        // The new connection gets its own strand
        auto self = http_listener<WKR>::shared_from_this();
        acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(
                &http_listener<WKR>::on_accept, self));
    }
};

template<typename WKR>
class http_server_impl
{
private:
    std::vector<std::thread> vt_;
    boost::asio::io_context  ioc_;
    boost::asio::signal_set  sig_;
    std::shared_ptr<http_listener<WKR>> pl_;

public:
    template<typename... SSArgs>
    http_server_impl(std::string const& address, unsigned short port, int nthread, std::string_view doc_root, bool verbose) : ioc_(nthread), sig_(ioc_, SIGINT, SIGTERM)
    {
        // Create and launch a listening port
        pl_ = std::make_shared<http_listener<WKR>>(
            ioc_,
            tcp::endpoint{ net::ip::make_address(address), port },
            doc_root,
            verbose);
        pl_->run();

#if defined(SIGQUIT)
        sig_.add(SIGQUIT);
#endif // defined(SIGQUIT)

        sig_.async_wait([&](beast::error_code const&, int)
            {
                // Stop the `io_context`. This will cause `run()`
                // to return immediately, eventually destroying the
                // `io_context` and all of the sockets in it.
                ioc_.stop();
            });

        // Run the I/O service on the requested number of threads
        vt_.reserve(nthread);
        for (auto i = 0; i < nthread; ++i)
            vt_.emplace_back(
                [&]
                {
                    ioc_.run();
                });
    }

    ~http_server_impl()
    {
        stop();
    }

    void stop()
    {
        ioc_.stop();
        for (auto& t : vt_)
        {
            if (t.joinable())
                t.join();
        }
        vt_.clear();
    }
};

