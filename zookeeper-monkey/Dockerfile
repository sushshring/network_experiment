# Step : Test and package
FROM maven:3.6-jdk-11-slim as target
WORKDIR /build
COPY pom.xml .
RUN mvn dependency:go-offline

COPY src/ /build/src/
RUN mvn package

# Step : Package image
FROM openjdk:11-jre-stretch
CMD exec java -jar /app/my-app.jar $ZOO_NODE /firstNode  dump-file
COPY --from=target /build/target/*jar-with-dependencies.jar /app/my-app.jar
