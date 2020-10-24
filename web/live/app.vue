<template>
  <div>this is live, <a href="/">go back to home</a></div>
</template>

<style>
  div {
    color: green;
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";

  @Component
  export default class extends Vue {
    mounted() {
      console.debug("hello live!");
    }
  }
</script>
