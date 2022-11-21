#include "MOE/orb_computer.h"
#include "MOE/flatbuffers/ocv_mat_generated.h"
#include "MOE/flatbuffers/ocv_kps_generated.h"
#include "MOE/orb_io.h"

#if CUDA_AVAILABLE
void OrbComputer::detect_and_compute(
    cv::InputArray _image,
    cv::InputArray _mask,
    std::vector<cv::KeyPoint> &_keypoints,
    cv::OutputArray _descriptors)
{
    cv::Mat gray;
    cv::cvtColor(_image, gray, cv::COLOR_RGB2GRAY);

    cv::cuda::GpuMat gpu_img(gray);
    //gpu_img.convertTo(gpu_img, CV_8UC1);
    //cv::cuda::GpuMat gpu_mask(_mask);
    cv::cuda::GpuMat gpu_mask;
    if(!_mask.empty()){
        gpu_mask.upload(_mask);
    }

    cv::cuda::GpuMat gpu_desc;
    if(!_descriptors.empty()){
        gpu_desc.upload(_descriptors);
    }
    the_orb->detectAndCompute(gpu_img, gpu_mask, _keypoints, gpu_desc);

    if(!gpu_desc.empty()){
        gpu_desc.download(_descriptors);
    }
}
#else
void OrbComputer::detect_and_compute(
    cv::InputArray _image,
    cv::InputArray _mask,
    std::vector<cv::KeyPoint> &_keypoints,
    cv::OutputArray _descriptors)
{
    the_orb->detectAndCompute(_image, _mask, _keypoints, _descriptors);
}
#endif

void OrbComputer::set_orb(int nfeatures,
                     double scaleFactor,
                     int nlevels,
                     int edgeThreshold,
                     int firstLevel,
                     int WTA_K,
                     cv::ORB::ScoreType scoreType,
                     int patchSize,
                     int fastThreshold)
{
    if (nfeatures != -1)
    {
        the_orb->setMaxFeatures(nfeatures);
    }
    if (scaleFactor != -1.0)
    {
        the_orb->setScaleFactor(scaleFactor);
    }
    if (nlevels != -1)
    {
        the_orb->setNLevels(nlevels);
    }
    if (edgeThreshold != -1)
    {
        the_orb->setEdgeThreshold(edgeThreshold);
    }
    if (firstLevel != -1)
    {
        the_orb->setFirstLevel(firstLevel);
    }
    if (WTA_K != -1)
    {
        the_orb->setWTA_K(WTA_K);
    }
    if (scoreType != (cv::ORB::ScoreType)-1)
    {
        the_orb->setScoreType(scoreType);
    }
    if (patchSize != -1)
    {
        the_orb->setPatchSize(patchSize);
    }
    if (fastThreshold != -1)
    {
        the_orb->setFastThreshold(fastThreshold);
    }
}

void OrbComputer::get_orb(int &nfeatures,
                     float &scaleFactor,
                     int &nlevels,
                     int &edgeThreshold,
                     int &firstLevel,
                     int &WTA_K,
                     cv::ORB::ScoreType &scoreType,
                     int &patchSize,
                     int &fastThreshold)
{
    nfeatures = the_orb->getMaxFeatures();
    scaleFactor = the_orb->getScaleFactor();
    nlevels = the_orb->getNLevels();
    edgeThreshold = the_orb->getEdgeThreshold();
    firstLevel = the_orb->getFirstLevel();
    WTA_K = the_orb->getWTA_K();
    scoreType = (cv::ORB::ScoreType)the_orb->getScoreType();
    patchSize = the_orb->getPatchSize();
    fastThreshold = the_orb->getFastThreshold();
}

void orb_loop(zmq::context_t * ctx){
    zmq::context_t * context;
    if(ctx!=NULL){
        context = static_cast<zmq::context_t*>(ctx);
    }
    else{
        context  = new zmq::context_t (1);
    }

    zmq::socket_t subscriber (*context, zmq::socket_type::sub);
    subscriber.bind("inproc://cam");
    zmq::socket_t publisher (*context, zmq::socket_type::pub);
    publisher.bind("inproc://orb");
    
    auto orb_cpu = OrbComputer();

    std::vector<cv::KeyPoint> kpts;
    cv::Mat desc;

    while(true){
        auto cvimg = read_mat(subscriber);

        orb_cpu.detect_and_compute(cvimg, cv::Mat(), kpts, desc);

        write_kps(publisher, kpts, desc);
    }

    if(ctx==NULL){
        free(context);
    }
}