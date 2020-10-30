<template>
  <div>
    {{ JSON.stringify(player) }}
  </div>
</template>

<style></style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";
  import { Prop } from "vue-property-decorator";

  @Component
  export default class extends Vue {
    @Prop() readonly player;
  }
</script>
