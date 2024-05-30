#include <cynta.h>

#include <stdbool.h>
#include <stdlib.h>

static void end_tracking(cynta_stream_t *stream)
{
    if (stream == NULL)
    {
        return;
    }

    free(stream->history);
    stream->history = NULL;
    stream->history_size = 0;
    stream->history_index = 0;

    free(stream->checkpoints);
    stream->checkpoints = NULL;
    stream->checkpoints_size = 0;
}

void cynta_stream_push_checkpoint(cynta_stream_t *stream)
{
    if (stream == NULL)
    {
        return;
    }

    // Push
    if ((stream->checkpoints = (size_t *)realloc(stream->checkpoints, sizeof(size_t) * (++stream->checkpoints_size))) == NULL)
    {
        end_tracking(stream);
        return;
    }
    stream->checkpoints[stream->checkpoints_size - 1] = stream->history_index;
}

void cynta_stream_rewind(cynta_stream_t *stream)
{
    if (stream == NULL ||
        stream->checkpoints_size == 0)
    {
        return;
    }

    // Pop
    stream->history_index = stream->checkpoints[stream->checkpoints_size - 1];
    stream->checkpoints_size--;
    if (stream->checkpoints_size == 0)
    {
        free(stream->checkpoints);
        stream->checkpoints = NULL;

        // At the peek of the history
        if (stream->history_index == stream->history_size)
        {
            free(stream->history);
            stream->history = NULL;
            stream->history_size = 0;
            stream->history_index = 0;
        }
    }
    else
    {
        if ((stream->checkpoints = (size_t *)realloc(stream->checkpoints, sizeof(size_t) * stream->checkpoints_size)) == NULL)
        {
            end_tracking(stream);
        }
    }
}

void cynta_stream_discard_checkpoint(cynta_stream_t *stream)
{
    if (stream == NULL ||
        stream->checkpoints_size == 0)
    {
        return;
    }

    // Discard
    stream->checkpoints_size--;
    if (stream->checkpoints_size == 0)
    {
        free(stream->checkpoints);
        stream->checkpoints = NULL;

        // At the peek of the history
        if (stream->history_index == stream->history_size)
        {
            free(stream->history);
            stream->history = NULL;
            stream->history_size = 0;
            stream->history_index = 0;
        }
    }
    else
    {
        if ((stream->checkpoints = (size_t *)realloc(stream->checkpoints, sizeof(size_t) * stream->checkpoints_size)) == NULL)
        {
            end_tracking(stream);
        }
    }
}

cynta_stream_error_t cynta_stream_next(cynta_stream_t *stream, uint8_t *out)
{
    if (stream == NULL ||
        out == NULL)
    {
        return CYNTA_STREAM_ERROR_NULL_POINTER;
    }

    uint8_t ret;
    if (stream->history != NULL &&
        stream->history_index < stream->history_size)
    {
        ret = stream->history[(stream->history_index)++];

        bool ran_out_of_history = stream->history_index == stream->history_size;
        bool no_longer_any_markers_left = stream->checkpoints_size == 0;
        if (ran_out_of_history && no_longer_any_markers_left)
        {
            free(stream->history);
            stream->history = NULL;
            stream->history_size = 0;
            stream->history_index = 0;
        }
    }
    else
    {
        cynta_stream_error_t err = stream->next(stream, &ret);
        if (err != CYNTA_STREAM_SUCCESS)
        {
            return err;
        }

        // Still need history
        if (stream->checkpoints_size != 0)
        {
            // Push
            if ((stream->history = (uint8_t *)realloc(stream->history, sizeof(uint8_t) * (++stream->history_size))) == NULL)
            {
                end_tracking(stream);
                return CYNTA_STREAM_ERROR_MEMORY_ALLOCATION;
            }
            stream->history[stream->history_size - 1] = ret;
            stream->history_index++;
        }
    }

    *out = ret;
    return CYNTA_STREAM_SUCCESS;
}

cynta_stream_error_t cynta_stream_init(cynta_stream_t *stream)
{
    if (stream == NULL)
    {
        return CYNTA_STREAM_ERROR_NULL_POINTER;
    }

    stream->history = NULL;
    stream->history_size = 0;
    stream->history_index = 0;

    stream->checkpoints = NULL;
    stream->checkpoints_size = 0;

    return CYNTA_STREAM_SUCCESS;
}

cynta_stream_error_t cynta_stream_deinit(cynta_stream_t *stream)
{
    if (stream == NULL)
    {
        return CYNTA_STREAM_ERROR_NULL_POINTER;
    }

    end_tracking(stream);

    return CYNTA_STREAM_SUCCESS;
}