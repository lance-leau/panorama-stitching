#include "pipeline.hh"

#include <stdexcept>

namespace panorama
{

    PipelineResult runPipeline(const std::vector<fs::path>&, const fs::path&)
    {
        // TODO cf proto en python:
        // load images (a pomper de fabrizio)
        // detect features
        // match features
        // estimate homographies
        // warp les images
        // blend les images pour creer le pano
        // save result
        throw std::logic_error("TODO");
    }

} // namespace panorama
