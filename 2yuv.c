#include <stdio.h>

#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

int main(int argc, char **argv)
{
    AVFormatContext *pFormatCtx;
    AVCodecParameters *pCodecPmt;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    AVPacket *packet;
    struct SwsContext *img_convert_ctx;

    int ret, used, got_picture, y_size;

    unsigned char *out_buffer;

    FILE *fp_yuv=fopen("output.yuv","wb+"); 

    char *filePath = "/home/yangfan/Devel/ffmpeg_test/bf.mp4";

    int i, videoindex = -1;

    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, filePath, NULL, NULL) != 0) {
        printf("couldn't open input stream.\n");
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("couldn't find stream information.\n");
        return -1;
    }
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    }

    av_dump_format(pFormatCtx, videoindex, filePath, 0);

    if (videoindex == -1) {
        printf("couldn't find a video stream.\n");
        return -1;
    }

    pCodecPmt = pFormatCtx->streams[videoindex]->codecpar;

    pCodec = avcodec_find_decoder(pCodecPmt->codec_id);
    if (pCodec == NULL) {
        printf("Codec not find.\n");
        return -1;
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);

    if (pCodecCtx == NULL) {
        printf("couldn't alloc codec context.\n");
        return -1;
    }

    avcodec_parameters_to_context(pCodecCtx, pCodecPmt);

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("couldn't open codec.\n");
        return -1;
    }

    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();

    out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));

    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

    packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    int stream_counter = 0;
    while(av_read_frame(pFormatCtx, packet) >= 0) {
        stream_counter += 1;
        if (packet->stream_index == videoindex) {

            ret = avcodec_send_packet(pCodecCtx, packet);
            if (ret < 0) {
                printf("sending packet for decoding.\n");
                return -1;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(pCodecCtx, pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    printf("reciving decoded frame error.\n");
                    return -1;
                } else {
                    sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
                    y_size = pCodecCtx->height * pCodecCtx->width;
                    fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);
                    fwrite(pFrameYUV->data[1], 1, y_size/4, fp_yuv);
                    fwrite(pFrameYUV->data[2], 1, y_size/4, fp_yuv);
                    printf("seccess decode one frame.\n");
                }

            }
        }
        av_packet_unref(packet);
    }

    fclose(fp_yuv);

    avcodec_free_context(&pCodecCtx);
    avformat_close_input(&pFormatCtx);

    printf("stream counter:%d\n", stream_counter);
    return 1;
}