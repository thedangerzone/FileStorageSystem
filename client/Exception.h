#pragma once
#include <iostream>  
#include <exception>  

constexpr std::string_view ERROR_MESSAGE = "Server responded with an error";

class ServerError : public std::exception {
public:
    ServerError(const std::string_view& reason = ERROR_MESSAGE) : reason(reason.data()) {}
    const char* what() const noexcept override {
        return reason;
    }

private:
    const char* reason;
};

class ClientError : public std::exception {
public:
    ClientError(const std::string_view& reason = ERROR_MESSAGE) : reason(reason.data()) {}
    const char* what() const noexcept override {
        return reason;
    }

private:
    const char* reason;
};
