#include "ElasticDegenerateString.h"

#include <algorithm>
#include <iterator>

ElasticDegenerateString ElasticDegenerateString::LoadFromString(std::istream &is) {

    // TODO pre-allocate the data_ member to only the size needed (needs extra function to check data characters)
    ElasticDegenerateString eds;

    // Reads variant (continuous string of data bearing characters) from the stream
    // TODO do not create a string copy, use iterators instead
    for (std::string chunk; std::getline(is, chunk, '{');) {
        if (chunk.size()) {
            eds.segment_index_.emplace_back(eds.variant_index_.size(), eds.variant_index_.size() + 1);
            eds.variant_index_.emplace_back(eds.data_.size(), eds.data_.size() + chunk.size());
            std::copy(std::begin(chunk), std::end(chunk), std::back_inserter(eds.data_));
            chunk.clear();
        }

        // Read all data in a segment and check for closing }
        if (!is.eof() && !std::getline(is, chunk, '}')) {
            throw std::logic_error("Malformed input! No closing '}' found in EDS string.");
        }
        if (!is.eof() && chunk.size() == 0) {
            throw std::logic_error("Malformed input! Empty segment '{}' found in EDS string.");
        }
        if (is.eof()) {
            break;
        }

        // Split chunk by ',' to a segment with multiple variants
        eds.segment_index_.emplace_back(eds.variant_index_.size(), eds.variant_index_.size());
        for (decltype(std::begin(chunk)) start = std::begin(chunk), match, pastend = std::next(std::end(chunk));
             (match = std::find(start, std::end(chunk), ',')), start != pastend; start = std::next(match)) {
            eds.segment_index_.back().second++;
            eds.variant_index_.emplace_back(eds.data_.size(), eds.data_.size() + std::distance(start, match));
            std::copy(start, match, std::back_inserter(eds.data_));
        }
    }

    return eds;
}

void ElasticDegenerateString::SaveToString(const ElasticDegenerateString &eds, std::ostream &os) {
    for (span_t s : eds.segment_index_) {
        size_t var_begin = std::get<0>(s);
        size_t var_end = std::get<1>(s);
        if (var_begin + 1 != var_end) {
            os << "{";
        }
        std::copy(std::next(std::begin(eds.data_), std::get<0>(eds.variant_index_.at(var_begin))),
                  std::next(std::begin(eds.data_), std::get<1>(eds.variant_index_.at(var_begin))),
                  std::ostream_iterator<char>(os));
        for (size_t v = var_begin + 1; v < var_end; v++) {
            os << ",";
            std::copy(std::next(std::begin(eds.data_), std::get<0>(eds.variant_index_.at(v))),
                      std::next(std::begin(eds.data_), std::get<1>(eds.variant_index_.at(v))),
                      std::ostream_iterator<char>(os));
        }
        if (var_begin + 1 != var_end) {
            os << "}";
        }
    }

}

std::ostream &operator<<(std::ostream &os, const ElasticDegenerateString &eds) {
    ElasticDegenerateString::SaveToString(eds,os);
    return os;
}

std::istream &operator>>(std::istream &is, ElasticDegenerateString &eds) {
    eds = ElasticDegenerateString::LoadFromString(is);
    return is;
}


