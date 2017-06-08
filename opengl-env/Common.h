#pragma once 

#include <cstdint>
#include <vector>
#include <random>

typedef std::uint32_t label_t;
typedef std::vector<float> vec_t;

const char endl[] = "\n";

#define MIN2(a, b)							((a) > (b) ? (b) : (a))
#define MAX2(a, b)							((a) > (b) ? (a) : (b))

/**
 *   Random Generator Class from tiny-dnn (https://github.com/tiny-dnn/tiny-dnn)
 */
class random_generator {
 public:
  static random_generator &get_instance() {
    static random_generator instance;
    return instance;
  }

  std::mt19937 &operator()() { return gen_; }

  void set_seed(unsigned int seed) { gen_.seed(seed); }

 private:
  // avoid gen_(0) for MSVC known issue
  // https://connect.microsoft.com/VisualStudio/feedback/details/776456
  random_generator() : gen_(1) {}
  std::mt19937 gen_;
};

template <typename T>
inline typename std::enable_if<std::is_integral<T>::value, T>::type
uniform_rand(T min, T max) {
  std::uniform_int_distribution<T> dst(min, max);
  return dst(random_generator::get_instance()());
}

template <typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, T>::type
uniform_rand(T min, T max) {
  std::uniform_real_distribution<T> dst(min, max);
  return dst(random_generator::get_instance()());
}

using Transition = std::tuple<vec_t,	// state (* input_frame_count_)
							label_t,	// action
							float,		// reward
							vec_t,		// next state (* input_frame_count_) --> size() == 0 if terminated
							float>;		// td

// end of file
