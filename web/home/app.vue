<template>
  <div>this is home, <a href="live">go to live</a></div>
</template>

<style>
  div {
    color: red;
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";
  import { Watch } from "vue-property-decorator";

  @Component
  export default class extends Vue {
    msg = "Hello";

    mounted() {
      console.debug("hello home!");
      console.debug(this.msg);
      this.msg = "yo";
    }

    @Watch("msg")
    onMsg() {
      console.debug("watch!");
    }
  }
</script>
