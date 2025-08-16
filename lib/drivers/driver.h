#pragma once

struct device {
  virtual void init() = 0;
  virtual void update() = 0;
};
