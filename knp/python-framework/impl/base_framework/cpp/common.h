/**
 * @file common.h
 * @brief Python bindings common header for framework traits.
 * @author Artiom N.
 * @date 21.02.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
#pragma once

#include <knp/core/message_endpoint.h>
#include <knp/core/uid.h>
#include <knp/framework/backend_loader.h>
#include <knp/framework/io/input_channel.h>
#include <knp/framework/io/output_channel.h>
#include <knp/framework/model.h>
#include <knp/framework/model_executor.h>
#include <knp/framework/monitoring/observer.h>
#include <knp/framework/network.h>
#include <knp/framework/sonata/network_io.h>

#include <string>

#include <boost/python.hpp>
#include <boost/python/implicit.hpp>


namespace py = boost::python;
namespace cpp_framework = knp::framework;

template <class Entity>
knp::core::UID get_entity_uid(Entity &self)
{
    return self.get_uid();
}
