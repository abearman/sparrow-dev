## Design Decisions

In this document, we present the design rationale for our exploration strategy as well as some key design decisions.

Upon receiving the project prompt from SAP, we quickly decided to focus on the enabling technology of autonomous drone navigation in GPS-denied environments as our main product. We were considering warehouse inventory management as our first application due to the simplicity of the environment, but later decided that drones would provide more value in environments with uneven terrain that terrestrial robots would not be able to traverse. To that end, we decided to shift our focus to another application: search and rescue operations.

Early on in our development process, we had multiple discussions about how we would perform the localization and potentially globalization needed for indoor drone navigation. We considered three key options: stereo vision using external cameras, onboard sonar and lidar sensors, and the Google Tango. Using triangulation using external cameras set up around a space initially seemed like a feasible option, but it soon became clear that this prevents much of the flexibility and portability we desired in our product. We then considered sonar and lidar, common means of onboard sensors to perform collision avoidance and to understand local space; this mechanism would give us the portability we desired and would allow us to get a 360 degree field of view. However, we ultimately decided to use the Google Tango. Even though it would only provide us with an 180 degree field of view, we felt that for our initial prototype, it would behoove us to use a preexisting technology that was specifically built with motion tracking, depth sensing, and localization capabilities.

[PID didn't work out so firmware]

[Decision to do an app]

[SAR specific items]

[UI-specific feedback from Cargi]
