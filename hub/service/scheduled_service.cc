#include "scheduled_service.h"

#include <iostream>
#include <memory>

#include <glog/logging.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind.hpp>

#include "service.h"

namespace hub {
namespace service {

void ScheduledService::start() {
  Service::start();

  _timer = std::make_unique<Timer>(_service);

  _timer->expires_from_now(_interval);
  _timer->async_wait(boost::bind(&ScheduledService::tick, this));
}

void ScheduledService::tick() {
  VLOG(3) << name() << "::tick()";

  auto ret = doTick();

  if (ret) {
    _timer->expires_from_now(_interval);
    _timer->async_wait(boost::bind(&ScheduledService::tick, this));
  }
}

void ScheduledService::stop() {
  _timer = nullptr;
  Service::stop();
}

}  // namespace service
}  // namespace hub