# Flow

C++14, header-only library for multi-stream data synchronization.

## What is this used for?

This library is meant for generating groups of data from separate series. The core problems it is meant to address are:

- How do we know what elements of data across multiple input streams relate to one other?
- How do we know when this data is ready to be retrieved ("captured") for further use?
- How do we capture different types of data uniformly and with minimal overhead?

In addressing these problems, this library enables data-driven event execution using data collected from distinct streaming series.

### Example use case

At Fetch Robotics Inc., this library is used in tandem with ROS. ROS subscribers are used to feed data into `Flow` capture buffers with light message feeding callbacks. The callbacks transfer ROS messages into the appropriate `Flow` capture buffer. `Flow` entities are serviced separately to compute events from these messages. The resulting data frames from synchronization contain all messages needed to run a particular task. In this way, messages are also used as a pace-setting mechanism for core execution blocks.
