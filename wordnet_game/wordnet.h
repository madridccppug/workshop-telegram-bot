
#pragma once

#include <string>
#include <wnb/core/wordnet.hh>

std::string random_q(const wnb::wordnet&wn, std::map<std::string, std::string>& q, int n_options);