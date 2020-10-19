<template>
  <h1 class="hello">Hello from example.vue</h1>
</template>

<style>
h1 {
  background-color: red;
}

.hello {
  color: aqua;
}
</style>

<script>
export default {
  mounted() {
    console.debug("hello there!");
  },
};
</script>
