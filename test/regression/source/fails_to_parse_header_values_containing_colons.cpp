/*
 * Copyright (c) 2013, 2014, 2015 Corvusoft
 */

//System Includes
#include <map>
#include <thread>
#include <string>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

//Project Includes
#include <restbed>

//External Includes
#include <catch.hpp>
#include <corvusoft/framework/http>

//System Namespaces
using std::thread;
using std::string;
using std::shared_ptr;
using std::make_shared;

//Project Namespaces
using namespace restbed;

//External Namespaces
using namespace framework;

void get_handler( const shared_ptr< Session >& session )
{
    const auto request = session->get_request( );
    CHECK( "Mozilla: 4.0" == request->get_header( "User-Agent" ) );
    
    session->close( 200 );
}

TEST_CASE( "fails to parse header values containing colons", "[session]" )
{
    auto resource = make_shared< Resource >( );
    resource->set_path( "test" );
    resource->set_method_handler( "GET", get_handler );
    
    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );
    
    Service service;
    service.publish( resource );
    
    thread service_thread( [ &service, settings ] ( )
    {
        service.start( settings );
    } );
    
    Http::Request request;
    request.method = "GET";
    request.port = 1984;
    request.host = "localhost";
    request.path = "/test";
    request.headers =
    {
        { "User-Agent", "Mozilla: 4.0" }
    };
    
    auto response = Http::get( request );
    
    REQUIRE( 200 == response.status_code );
    
    service.stop( );
    service_thread.join( );
}
