FROM timescale/timescaledb:latest-pg9.6
ENV POSTGRES_USER root
ENV POSTGRES_PASSWORD .root
ENV POSTGRES_DB DynData
COPY initdb.sql /docker-entrypoint-initdb.d/
RUN ["bash"]