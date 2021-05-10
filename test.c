#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>




int main(void) {
    const char *infoStr = NULL;
    const char *name;
    void *op = NULL;
    const AVOutputFormat *muxer;
    const AVInputFormat *demuxer;
    const AVCodec *codec;
    const AVFilter *filter;

    infoStr = avcodec_configuration();
    printf("\n<config info>\n%s\n", infoStr);

    printf("\n<protocol list>\n");
    while((name = avio_enum_protocols(&op, 0)))
        printf("\t[in protocol]%s\n", name);

    op = NULL;
    while((name = avio_enum_protocols(&op, 1)))
        printf("\t[out protocol] %s\n", name);

    op = NULL;

    printf("\n<muxer list>\n");
    while((muxer = av_muxer_iterate(&op)))
        printf("\t[muxer]%s\n", muxer->name);

    op = NULL;
    while((demuxer = av_demuxer_iterate(&op)))
        printf("\t[demuxer]%s\n", demuxer->name);

    op = NULL;
    printf("\n<codec list>\n");
    while((codec = av_codec_iterate(&op))) {
        printf("\t[codec]%s\t", codec->name);
        if (codec->decode != NULL)
            printf("[enc]");
        else
            printf("[dec]");
        switch(codec->type) {
            case AVMEDIA_TYPE_UNKNOWN:
                printf("[unknown type]\n");
                break;
            case AVMEDIA_TYPE_VIDEO:
                printf("[video]\n");
                break;
            case AVMEDIA_TYPE_AUDIO:
                printf("[audio]\n");
                break;
            case AVMEDIA_TYPE_DATA:
                printf("[data]\n");
                break;
            case AVMEDIA_TYPE_SUBTITLE:
                printf("[subtitle]\n");
                break;
            case AVMEDIA_TYPE_ATTACHMENT:
                printf("[attachment]\n");
                break;
            case AVMEDIA_TYPE_NB:
                printf("[nb]\n");
                break;
            default:
                printf("[what?????]\n");
                break;
        }
    }

    op = NULL;
    printf("\n<filter list>\n");
    while ((filter = av_filter_iterate(&op))) {
        printf("[filter]%s\n", filter->name);
    }
}
