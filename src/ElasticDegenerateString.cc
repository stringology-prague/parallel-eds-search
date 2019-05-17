#include "ElasticDegenerateString.h"

#include <algorithm>
#include <iterator>

ElasticDegenerateString ElasticDegenerateString::LoadString(std::istream &is) {

    // TODO pre-allocate the data_ member to only the size needed (needs extra function to check data characters)
    ElasticDegenerateString eds;

    // Reads variant (continuous string of data bearing characters) from the stream
    // TODO do not create a string copy, use iterators instead
    for (std::string chunk; std::getline(is, chunk, '{');) {
        if (chunk.size()) {
            std::cout << "read single chunk, size: " << chunk.size() << ", data: " << chunk << std::endl;
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
        if (is.eof()){
            break;
        }

        // Split chunk by ',' to a segment with multiple variants
        eds.segment_index_.emplace_back(eds.variant_index_.size(), eds.variant_index_.size());
        for (decltype(std::begin(chunk)) start = std::begin(chunk), match, pastend = std::next(std::end(chunk));
             (match = std::find(start, std::end(chunk), ',')), start != pastend; start = std::next(match)) {
            std::string variant(start, match);
            std::cout << "read variant, size: " << variant.size() << ", data: " << variant << std::endl;
            eds.segment_index_.back().second++;
            eds.variant_index_.emplace_back(eds.data_.size(), eds.data_.size() + variant.size());
            std::copy(start,match,std::back_inserter(eds.data_));
        }
    }

    return eds;
}

std::ostream & operator << (std::ostream & os, const ElasticDegenerateString &eds)
{
    os << "DATA: " << eds.data_.size() << std::endl;
    std::copy(std::begin(eds.data_), std::end(eds.data_), std::ostream_iterator<char>(os));
    os << std::endl;
    os << "VARIANT INDEX: " << eds.variant_index_.size() << std::endl;
    for (auto t : eds.variant_index_){
        os << t.first << ".." << t.second << " ";
    }
    os << std::endl;
    os << "SEGMENT INDEX: " << eds.segment_index_.size() << std::endl;
    for (auto t : eds.segment_index_){
        os << t.first << ".." << t.second << " ";
    }
    return os;
}

std::istream & operator >> (std::istream & is, ElasticDegenerateString &eds)
{
    eds = ElasticDegenerateString::LoadString(is);
    return is;
}