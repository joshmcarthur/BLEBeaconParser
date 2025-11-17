FROM platformio/platformio-core:latest

WORKDIR /project

# Copy project files
COPY . /project/

# Install dependencies and run tests
RUN pio test -e native

CMD ["pio", "test", "-e", "native"]

