#include "were1_unix_socket.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <alsa/asoundlib.h>
#include <alsa/pcm_external.h>
#include <linux/soundcard.h>


#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>


#define sparkle_server "/data/data/com.sion.sparkle/files/audio-0" //XXX2
#define sparkle_period_size (65536 / 8)
#define sparkle_periods 8
#define sparkle_buffer_size (sparkle_period_size * sparkle_periods)
#define sparkle_max_data_size sparkle_period_size


typedef struct snd_pcm_sparkle {
    snd_pcm_ioplug_t io;
    //char *device;
    int fd;
    //int fragment_set;
    //int caps;
    //int format;
    //unsigned int period_shift;
    //unsigned int periods;
    unsigned int frame_bytes;
    uint64_t pointer;
} snd_pcm_sparkle_t;






static void critical(int ret)
{
    if (ret == -1)
        abort();
}

static int sparkle_connect(snd_pcm_sparkle_t *sparkle)
{
    int fd = were1_unix_socket_connect(sparkle_server);
    if (fd == -1)
        return -1;

    sparkle->fd = fd;

    return 0;
}

static int sparkle_disconnect(snd_pcm_sparkle_t *sparkle)
{
    were1_unix_socket_destroy(sparkle->fd);

    sparkle->fd = -1;

    return 0;
}

static void sparkle_send_start(snd_pcm_sparkle_t *sparkle)
{
    uint64_t code = 1;
    critical(were1_unix_socket_send_all(sparkle->fd, &code, sizeof(uint64_t)));
}

static void sparkle_send_stop(snd_pcm_sparkle_t *sparkle)
{
    uint64_t code = 2;
    critical(were1_unix_socket_send_all(sparkle->fd, &code, sizeof(uint64_t)));
}

static void sparkle_send_data(snd_pcm_sparkle_t *sparkle, const void *data, int size)
{
    uint64_t code = 3;
    uint64_t size__ = size;
    critical(were1_unix_socket_send_all(sparkle->fd, &code, sizeof(uint64_t)));
    critical(were1_unix_socket_send_all(sparkle->fd, &size__, sizeof(uint64_t)));
    critical(were1_unix_socket_send_all(sparkle->fd, data, size));
}

static void sparkle_receive_pointer(snd_pcm_sparkle_t *sparkle)
{
    uint64_t data;

    while (were1_unix_socket_bytes_available(sparkle->fd) >= sizeof(uint64_t))
    {
        critical(were1_unix_socket_receive_all(sparkle->fd, &data, sizeof(uint64_t)));
        sparkle->pointer = data;
    }
}

static int sparkle_poll_revents(snd_pcm_ioplug_t *io,
                    struct pollfd *pfd,
                    unsigned int nfds,
                    unsigned short *revents)
{
    snd_pcm_sparkle_t *sparkle = io->private_data;

    sparkle_receive_pointer(sparkle);

    //*revents = io->stream == SND_PCM_STREAM_PLAYBACK ? POLLOUT : POLLIN;
    *revents = POLLOUT;

    return 0;
}

static snd_pcm_sframes_t sparkle_write(snd_pcm_ioplug_t *io,
                   const snd_pcm_channel_area_t *areas,
                   snd_pcm_uframes_t offset,
                   snd_pcm_uframes_t size)
{
#if 0
    snd_pcm_oss_t *oss = io->private_data;
    const char *buf;
    ssize_t result;

    /* we handle only an interleaved buffer */
    buf = (char *)areas->addr + (areas->first + areas->step * offset) / 8;
    size *= oss->frame_bytes;
    result = write(oss->fd, buf, size);
    if (result <= 0)
        return result;
    return result / oss->frame_bytes;
#endif

    snd_pcm_sparkle_t *sparkle = io->private_data;

    const char *data = (char *)areas->addr + (areas->first + areas->step * offset) / 8;

    int size_bytes = size * sparkle->frame_bytes;

    if (size_bytes > sparkle_period_size)
        size_bytes = sparkle_period_size;

    sparkle_send_data(sparkle, data, size_bytes);

    return size_bytes / sparkle->frame_bytes;
}

static snd_pcm_sframes_t sparkle_read(snd_pcm_ioplug_t *io,
                  const snd_pcm_channel_area_t *areas,
                  snd_pcm_uframes_t offset,
                  snd_pcm_uframes_t size)
{
#if 0
    snd_pcm_oss_t *oss = io->private_data;
    char *buf;
    ssize_t result;

    /* we handle only an interleaved buffer */
    buf = (char *)areas->addr + (areas->first + areas->step * offset) / 8;
    size *= oss->frame_bytes;
    result = read(oss->fd, buf, size);
    if (result <= 0)
        return result;
    return result / oss->frame_bytes;
#endif

    return size;
}

static snd_pcm_sframes_t sparkle_pointer(snd_pcm_ioplug_t *io)
{
#if 0
    snd_pcm_oss_t *oss = io->private_data;
    struct count_info info;
    int ptr;

    if (ioctl(oss->fd, io->stream == SND_PCM_STREAM_PLAYBACK ?
          SNDCTL_DSP_GETOPTR : SNDCTL_DSP_GETIPTR, &info) < 0) {
        fprintf(stderr, "*** OSS: oss_pointer error\n");
        return 0;
    }
    ptr = snd_pcm_bytes_to_frames(io->pcm, info.ptr);
    return ptr;
#endif

    snd_pcm_sparkle_t *sparkle = io->private_data;

    sparkle_receive_pointer(sparkle);

    return sparkle->pointer / sparkle->frame_bytes;
}

static int sparkle_start(snd_pcm_ioplug_t *io)
{
#if 0
    snd_pcm_oss_t *oss = io->private_data;
    int tmp = io->stream == SND_PCM_STREAM_PLAYBACK ?
        PCM_ENABLE_OUTPUT : PCM_ENABLE_INPUT;

    if (ioctl(oss->fd, SNDCTL_DSP_SETTRIGGER, &tmp) < 0) {
        fprintf(stderr, "*** OSS: trigger failed\n");
        if (io->stream == SND_PCM_STREAM_CAPTURE)
            /* fake read to trigger */
            read(oss->fd, &tmp, 0);
    }
    return 0;
#endif

    snd_pcm_sparkle_t *sparkle = io->private_data;

    sparkle->pointer = 0;

    sparkle_send_start(sparkle);

    return 0;
}

static int sparkle_stop(snd_pcm_ioplug_t *io)
{
#if 0
    snd_pcm_oss_t *oss = io->private_data;
    int tmp = 0;

    ioctl(oss->fd, SNDCTL_DSP_SETTRIGGER, &tmp);
    return 0;
#endif

    snd_pcm_sparkle_t *sparkle = io->private_data;

    sparkle->pointer = 0;

    sparkle_send_stop(sparkle);

    return 0;
}

#if 0
static int oss_drain(snd_pcm_ioplug_t *io)
{
    snd_pcm_oss_t *oss = io->private_data;

    if (io->stream == SND_PCM_STREAM_PLAYBACK)
        ioctl(oss->fd, SNDCTL_DSP_SYNC);
    return 0;
}
#endif

#if 0
static int oss_prepare(snd_pcm_ioplug_t *io)
{
    snd_pcm_oss_t *oss = io->private_data;
    int tmp;

    ioctl(oss->fd, SNDCTL_DSP_RESET);

    tmp = io->channels;
    if (ioctl(oss->fd, SNDCTL_DSP_CHANNELS, &tmp) < 0) {
        perror("SNDCTL_DSP_CHANNELS");
        return -EINVAL;
    }
    tmp = oss->format;
    if (ioctl(oss->fd, SNDCTL_DSP_SETFMT, &tmp) < 0) {
        perror("SNDCTL_DSP_SETFMT");
        return -EINVAL;
    }
    tmp = io->rate;
    if (ioctl(oss->fd, SNDCTL_DSP_SPEED, &tmp) < 0 ||
        tmp > io->rate * 1.01 || tmp < io->rate * 0.99) {
        perror("SNDCTL_DSP_SPEED");
        return -EINVAL;
    }
    return 0;
}
#endif


static int sparkle_hw_params(snd_pcm_ioplug_t *io,
             snd_pcm_hw_params_t *params ATTRIBUTE_UNUSED)
{
    snd_pcm_sparkle_t *sparkle = io->private_data;

    sparkle->frame_bytes = (snd_pcm_format_physical_width(io->format) * io->channels) / 8;

    switch (io->format)
    {
        case SND_PCM_FORMAT_S16_LE:
            break;
        default:
            fprintf(stderr, "sparkle: unsupported format %s\n", snd_pcm_format_name(io->format));
            return -EINVAL;
    }

    fprintf(stdout,
        "buffer_size_frames=%ld  period_size_frames=%ld frame_size_bytes=%d\n",
        io->buffer_size,         io->period_size,       sparkle->frame_bytes);

    return 0;
}


#define ARRAY_SIZE(ary)    (sizeof(ary)/sizeof(ary[0]))

static int sparkle_hw_constraint(snd_pcm_sparkle_t *sparkle)
{
    snd_pcm_ioplug_t *io = &sparkle->io;

    static const snd_pcm_access_t access_list[] = {
        SND_PCM_ACCESS_RW_INTERLEAVED,
        //SND_PCM_ACCESS_MMAP_INTERLEAVED,
    };

#if 0
    /* period and buffer bytes must be power of two */
    static const unsigned int bytes_list[] = {
        1U<<8, 1U<<9, 1U<<10, 1U<<11, 1U<<12, 1U<<13, 1U<<14, 1U<<15,
        1U<<16, 1U<<17, 1U<<18, 1U<<19, 1U<<20, 1U<<21, 1U<<22, 1U<<23
    };
#endif

    static const unsigned int formats[] = {
        SND_PCM_FORMAT_S16_LE
    };

    int err;

    // Access type
    err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_ACCESS, ARRAY_SIZE(access_list), access_list);
    if (err < 0)
        return err;

    // Formats
    //err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_FORMAT, nformats, format);
    err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_FORMAT, ARRAY_SIZE(formats), formats);
    if (err < 0)
        return err;

    // Channels
    //err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_CHANNELS, nchannels, channel);
    err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_CHANNELS, 2, 2);
    if (err < 0)
        return err;

    // Rates
    err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_RATE, 48000, 48000);
    if (err < 0)
        return err;

    // Period size
    //err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_PERIOD_BYTES, ARRAY_SIZE(bytes_list), bytes_list);
    //err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_PERIOD_BYTES, 1U<<16, 1U<<16);
    err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_PERIOD_BYTES, sparkle_period_size, sparkle_period_size);
    if (err < 0)
        return err;

    // Periods
    //err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_PERIODS, 2, 1024);
    //err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_PERIODS, 4, 4);
    err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_PERIODS, sparkle_periods, sparkle_periods);
    if (err < 0)
        return err;

    // Buffer size
    //err = snd_pcm_ioplug_set_param_list(io, SND_PCM_IOPLUG_HW_BUFFER_BYTES, ARRAY_SIZE(bytes_list), bytes_list);
    //err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_BUFFER_BYTES, 1U<<18, 1U<<18);
    err = snd_pcm_ioplug_set_param_minmax(io, SND_PCM_IOPLUG_HW_BUFFER_BYTES, sparkle_buffer_size, sparkle_buffer_size);
    if (err < 0)
        return err;

    return 0;
}


static int sparkle_close(snd_pcm_ioplug_t *io)
{
    snd_pcm_sparkle_t *sparkle = io->private_data;

    sparkle_disconnect(sparkle);
    //free(oss->device);
    free(sparkle);

    return 0;
}

static const snd_pcm_ioplug_callback_t sparkle_playback_callback = {
    .start = sparkle_start,
    .stop = sparkle_stop,
    .transfer = sparkle_write,
    .pointer = sparkle_pointer,
    .close = sparkle_close,
    .hw_params = sparkle_hw_params,
    //.prepare = oss_prepare,
    //.drain = oss_drain,
    .poll_revents = sparkle_poll_revents,
};

static const snd_pcm_ioplug_callback_t sparkle_capture_callback = {
    .start = sparkle_start,
    .stop = sparkle_stop,
    .transfer = sparkle_read,
    .pointer = sparkle_pointer,
    .close = sparkle_close,
    .hw_params = sparkle_hw_params,
    //.prepare = oss_prepare,
    //.drain = oss_drain,
    .poll_revents = sparkle_poll_revents,
};


SND_PCM_PLUGIN_DEFINE_FUNC(sparkle)
{
    snd_config_iterator_t i, next;
    //const char *device = "/dev/dsp";
    int err;
    snd_pcm_sparkle_t *sparkle;

    snd_config_for_each(i, next, conf)
    {
        snd_config_t *n = snd_config_iterator_entry(i);
        const char *id;

        if (snd_config_get_id(n, &id) < 0)
            continue;

        if (strcmp(id, "comment") == 0 || strcmp(id, "type") == 0 || strcmp(id, "hint") == 0)
            continue;
#if 0
        if (strcmp(id, "device") == 0)
        {
            if (snd_config_get_string(n, &device) < 0)
            {
                SNDERR("Invalid type for %s", id);
                return -EINVAL;
            }
            continue;
        }
#endif

        SNDERR("Unknown field %s", id);
        return -EINVAL;
    }

    sparkle = calloc(1, sizeof(*sparkle));
    if (!sparkle)
    {
        SNDERR("cannot allocate");
        return -ENOMEM;
    }

#if 0
    oss->device = strdup(device);
    if (oss->device == NULL) {
        SNDERR("cannot allocate");
        free(oss);
        return -ENOMEM;
    }
    oss->fd = open(device, stream == SND_PCM_STREAM_PLAYBACK ?
               O_WRONLY : O_RDONLY);
    if (oss->fd < 0) {
        err = -errno;
        SNDERR("Cannot open device %s", device);
        goto error;
    }
#endif

    if (sparkle_connect(sparkle) == -1)
    {
        err = -errno;
        SNDERR("cannot connect");
        goto error;
    }

    sparkle->io.version = SND_PCM_IOPLUG_VERSION;
    sparkle->io.name = "Sparkle PCM I/O Plugin";
    sparkle->io.poll_fd = sparkle->fd;
    //sparkle->io.poll_events = stream == SND_PCM_STREAM_PLAYBACK ? POLLOUT : POLLIN;
    sparkle->io.poll_events = POLLIN;
    sparkle->io.mmap_rw = 0;
    sparkle->io.callback = stream == SND_PCM_STREAM_PLAYBACK ?
        &sparkle_playback_callback : &sparkle_capture_callback;
    sparkle->io.private_data = sparkle;

    err = snd_pcm_ioplug_create(&sparkle->io, name, stream, mode);
    if (err < 0)
        goto error;

    if ((err = sparkle_hw_constraint(sparkle)) < 0)
    {
        snd_pcm_ioplug_delete(&sparkle->io);
        return err;
    }

    *pcmp = sparkle->io.pcm;

    return 0;

 error:
    if (sparkle->fd != -1)
        sparkle_disconnect(sparkle);
    //free(sparkle->device);
    free(sparkle);

    return err;
}

SND_PCM_PLUGIN_SYMBOL(sparkle);
