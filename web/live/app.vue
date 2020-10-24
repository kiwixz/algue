<template>
  <div>this is live, <a href="/">go back to home</a></div>
</template>

<style>
  div {
    color: green;
  }
</style>

<script>
  export default {
    mounted() {
      console.debug("hello live!");
    },
  };
</script>
